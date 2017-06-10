#include "stdafx.h"
#include "FLVDecode.h"

namespace FLVPlayer
{
	FLVDecode::FLVDecode(HWND hWND)
		:m_hWND(hWND),
		m_audioQueue(m_lock),
		m_videoQueue(m_lock),
		m_videoTask(*this),
		m_audioTask(*this),
		m_audioRender(m_audioTask),
		m_videoRender(m_videoTask),
		m_baseTime(-1),
		m_basePTS(-1),
		m_bFirstI(FALSE)
	{
		m_aac.Reset(new AACDecode());
		m_h264.Reset(new H264Decode());
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}

	FLVDecode::~FLVDecode()
	{

	}
	BOOL FLVDecode::Submit()
	{
		if (m_reader.OpenURL("rtmp://live.hkstv.hk.lxdns.com/live/hks"))
		//if (m_reader.OpenURL("rtmp://10.121.86.127/live/test_360p"))
		{
			m_size.cx = static_cast<LONG>(m_reader.GetScript().width);
			m_size.cy = static_cast<LONG>(m_reader.GetScript().height);
			if (TinyTaskBase::Submit(BindCallback(&FLVDecode::OnMessagePump, this)))
			{
				m_audioRender.Submit();
				m_audioTask.Submit();
				m_videoRender.Submit();
				m_videoTask.Submit();
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL FLVDecode::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		m_audioRender.Close(dwMS);
		m_videoRender.Close(dwMS);
		m_videoTask.Close(dwMS);
		m_audioTask.Close(dwMS);
		return TinyTaskBase::Close(dwMS);
	}
	void FLVDecode::OnMessagePump()
	{
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				break;
			}
			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFirstI)
						{
							SAFE_DELETE_ARRAY(block.audio.data);
							SAFE_DELETE_ARRAY(block.video.data);
							continue;
						}
						SampleTag tag = { 0 };
						tag.bits = block.audio.data;
						tag.size = block.audio.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_audioQueue.Push(tag);
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						if (!m_h264->Initialize(m_size, m_size))
						{
							goto _ERROR;
						}
						if (!m_h264->Open(block.video.data, block.video.size))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFirstI)
						{
							if (block.video.codeType != 1)
							{
								SAFE_DELETE_ARRAY(block.audio.data);
								SAFE_DELETE_ARRAY(block.video.data);
								continue;
							}
							m_bFirstI = TRUE;
						}
						SampleTag tag = { 0 };
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_videoQueue.Push(tag);
					}
				}
			}
		}
	_ERROR:
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}

	//////////////////////////////////////////////////////////////////////////
	FLVAudioRender::FLVAudioRender(FLVVAudioTask& decode)
		:m_decode(decode),
		m_bInitialize(FALSE),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent();
	}
	FLVAudioRender::~FLVAudioRender()
	{

	}
	BOOL FLVAudioRender::Submit()
	{
		if (!m_player.Initialize(m_decode.m_decode.m_hWND))
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&FLVAudioRender::OnMessagePump, this));
	}
	BOOL FLVAudioRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVAudioRender::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			if (!m_bInitialize)
			{
				m_timer.BeginTime();
				m_bInitialize = TRUE;
				if (!m_player.SetFormat(&m_decode.m_decode.m_aac->GetFormat(), tag.size * 2))
					break;
				m_events[0].CreateEvent(TRUE, FALSE);
				m_events[1].CreateEvent(TRUE, FALSE);
				DSBPOSITIONNOTIFY vals[2];
				vals[0].dwOffset = tag.size - 1;
				vals[0].hEventNotify = m_events[0];
				vals[1].dwOffset = tag.size * 2 - 1;
				vals[1].hEventNotify = m_events[1];
				m_player.SetPositions(2, vals);
				m_timer.EndTime();
				m_decode.m_decode.m_lockTime.Lock();
				m_decode.m_decode.m_baseTime += m_timer.GetMillisconds();
				m_decode.m_decode.m_lockTime.Unlock();
				DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
				INT offset = tag.samplePTS - dwMS;
				Sleep(offset < 0 ? 0 : offset);
				if (tag.size != 4096)
				{
					m_player.Fill(tag.bits, tag.size);
				}
				m_player.Fill(tag.bits, tag.size);
				SAFE_DELETE_ARRAY(tag.bits);
				m_player.Play();
			}
			else
			{
				m_player.Fill(tag.bits, tag.size);
				SAFE_DELETE_ARRAY(tag.bits);
			}
			HANDLE handles[2] = { m_events[0],m_events[1] };
			WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoRender::FLVVideoRender(FLVVideoTask& decode)
		:m_decode(decode),
		m_wPTS(0),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoRender::~FLVVideoRender()
	{

	}
	BOOL FLVVideoRender::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVideoRender::OnMessagePump, this));
	}
	BOOL FLVVideoRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVVideoRender::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
			INT offset = tag.samplePTS - dwMS;
			Sleep(offset < 0 ? 0 : offset);
			OnRender(tag.bits, tag.size);
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}

	void FLVVideoRender::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 4);
		HDC hDC = GetDC(m_decode.m_decode.m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_decode.m_decode.m_size.cx;
			bmi.bmiHeader.biHeight = -m_decode.m_decode.m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 4;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap != NULL)
			{
				memcpy(pvBits, bits, size);
				TinyMemDC dc(hDC, hBitmap);
				TinyRectangle src = { 0,0, m_decode.m_decode.m_size.cx, m_decode.m_decode.m_size.cy };
				dc.Render(src, src, FALSE);
				SAFE_DELETE_OBJECT(hBitmap);
			}
			ReleaseDC(m_decode.m_decode.m_hWND, hDC);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoTask::FLVVideoTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoTask::~FLVVideoTask()
	{

	}
	BOOL FLVVideoTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVideoTask::OnMessagePump, this));
	}
	BOOL FLVVideoTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE || m_decode.m_videoQueue.IsEmpty())
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_videoQueue.Pop();
			if (tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_h264->Decode(tag, bo, so))
				{
					SAFE_DELETE_ARRAY(tag.bits);
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					tag.samplePTS = m_decode.m_h264->GetYUV420()->pkt_pts;
					tag.sampleDTS = tag.samplePTS;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVAudioTask::FLVVAudioTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVAudioTask::~FLVVAudioTask()
	{

	}
	BOOL FLVVAudioTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVAudioTask::OnMessagePump, this));
	}
	BOOL FLVVAudioTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE || m_decode.m_audioQueue.IsEmpty())
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_audioQueue.Pop();
			if (tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_aac->Decode(tag.bits, tag.size, bo, so))
				{
					SAFE_DELETE_ARRAY(tag.bits);
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}
}

