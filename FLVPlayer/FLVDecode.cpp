#include "stdafx.h"
#include "FLVDecode.h"

namespace FLVPlayer
{
	FLVDecode::FLVDecode(HWND hWND)
		:m_hWND(hWND),
		m_audioQueue(m_lock),
		m_videoQueue(m_lock),
		m_decodeTask(*this),
		m_audioRender(*this),
		m_videoRender(m_decodeTask),
		m_dwBaseMS(0)
	{
		m_aac.Reset(new AACDecode());
		m_h264.Reset(new H264Decode());
	}

	FLVDecode::~FLVDecode()
	{

	}
	BOOL FLVDecode::Submit()
	{
		if (m_reader.Open("D:\\3.flv"))
		{
			m_size.cx = static_cast<LONG>(m_reader.GetScript().width);
			m_size.cy = static_cast<LONG>(m_reader.GetScript().height);
			if (TinyTaskBase::Submit(BindCallback(&FLVDecode::OnMessagePump, this)))
			{
				m_audioRender.Submit();
				m_videoRender.Submit();
				m_decodeTask.Submit();
			}
		}
		return FALSE;
	}
	BOOL FLVDecode::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVDecode::OnMessagePump()
	{
		FLV_BLOCK block = { 0 };
		m_dwBaseMS = timeGetTime();
		for (;;)
		{
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
	FLVAudioRender::FLVAudioRender(FLVDecode& decode)
		:m_decode(decode),
		m_bInitialize(FALSE),
		m_bFlag(FALSE),
		m_dwMS(0)
	{

	}
	FLVAudioRender::~FLVAudioRender()
	{

	}
	BOOL FLVAudioRender::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVAudioRender::OnMessagePump, this));
	}
	BOOL FLVAudioRender::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVAudioRender::OnMessagePump()
	{
		for (;;)
		{
			SampleTag tag = m_decode.m_audioQueue.Pop();
			if (tag.size <= 0)
			{
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_decode.m_aac->Decode(tag.bits, tag.size, bo, so))
			{
				DWORD dwMS = timeGetTime() - m_decode.m_dwBaseMS;
				INT offset = tag.samplePTS - dwMS;
				TRACE("audio:%d\n", offset);
				Sleep(offset < 0 ? 0 : offset);
				if (!m_bInitialize)
				{
					m_bInitialize = TRUE;
					if (!m_player.Initialize(m_decode.m_hWND, &m_decode.m_aac->GetFormat(), so * 2))
						break;
					m_events[0].CreateEvent();
					m_events[1].CreateEvent();
					DSBPOSITIONNOTIFY vals[2];
					vals[0].dwOffset = so - 1;
					vals[0].hEventNotify = m_events[0];
					vals[1].dwOffset = so * 2 - 1;
					vals[1].hEventNotify = m_events[1];
					if (m_player.SetPositions(2, vals))
					{
						m_player.Play();
						if (so != 4096)
						{
							m_player.Fill(bo, so);
						}
					}
				}
				m_player.Fill(bo, so);
			}
			HANDLE handles[2] = { m_events[0],m_events[1] };
			HRESULT hRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (hRes >= WAIT_OBJECT_0 && hRes <= (WAIT_OBJECT_0 + 1))
			{
				SAFE_DELETE_ARRAY(tag.bits);
				continue;
			}
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoRender::FLVVideoRender(FLVDecodeTask& decode)
		:m_decode(decode),
		m_wPTS(0),
		m_bFlag(FALSE),
		m_dwMS(0)
	{

	}
	FLVVideoRender::~FLVVideoRender()
	{

	}
	BOOL FLVVideoRender::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVideoRender::OnMessagePump, this));
	}
	BOOL FLVVideoRender::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVVideoRender::OnMessagePump()
	{
		for (;;)
		{
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
			{
				continue;
			}
			DWORD dwMS = timeGetTime() - m_decode.m_decode.m_dwBaseMS;
			INT offset = tag.samplePTS - dwMS;
			TRACE("video:%d\n", offset);
			Sleep(offset < 0 ? 0 : offset);
			OnRender(tag.bits, tag.size);
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}

	void FLVVideoRender::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 3);
		HDC hDC = GetDC(m_decode.m_decode.m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_decode.m_decode.m_size.cx;
			bmi.bmiHeader.biHeight = -m_decode.m_decode.m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 3;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap)
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
	FLVDecodeTask::FLVDecodeTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{

	}
	FLVDecodeTask::~FLVDecodeTask()
	{

	}
	BOOL FLVDecodeTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVDecodeTask::OnMessagePump, this));
	}
	BOOL FLVDecodeTask::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVDecodeTask::OnMessagePump()
	{
		for (;;)
		{
			INT size = m_queue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(1);
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

