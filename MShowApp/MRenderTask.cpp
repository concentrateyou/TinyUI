#include "stdafx.h"
#include "MRenderTask.h"

namespace MShow
{
	MRenderTask::MRenderTask()
		:m_bInitializeA(FALSE),
		m_bInitializeV(FALSE),
		m_hWND(NULL),
		m_videoQueue(m_lock)

	{

	}
	MRenderTask::~MRenderTask()
	{

	}
	BOOL MRenderTask::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
		m_hWND = hWND;
		if (!m_splayer.Initialize(hWND))
			return  FALSE;
		if (!m_dx.Initialize(hWND, cx, cy))
			return FALSE;
		m_aac.Reset(new AACDecode());
		m_h264.Reset(new H264Decode());
		m_close.CreateEvent();
		return TRUE;
	}
	BOOL MRenderTask::Submit(LPCSTR pzURL)
	{
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG, FLV_PACKET*)>(this, &MRenderTask::OnAudio));
		m_reader.EVENT_AUDIO += m_onAudio;
		m_onVideo.Reset(new Delegate<void(BYTE*, LONG, FLV_PACKET*)>(this, &MRenderTask::OnVideo));
		m_reader.EVENT_VIDEO += m_onVideo;
		m_onScript.Reset(new Delegate<void(FLV_SCRIPTDATA*)>(this, &MRenderTask::OnScript));
		m_reader.EVENT_SCRIPT += m_onScript;
		if (m_reader.Open(pzURL))
		{
			return TinyTaskBase::Submit(BindCallback(&MRenderTask::OnMessagePump, this));
		}
		return FALSE;
	}

	BOOL MRenderTask::Close(DWORD dwMS)
	{
		m_reader.EVENT_AUDIO -= m_onAudio;
		m_reader.EVENT_VIDEO -= m_onVideo;
		m_reader.EVENT_SCRIPT -= m_onScript;
		m_reader.Close(INFINITE);
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	void MRenderTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_videoQueue.GetCount() > 100)
				break;
		}
		DWORD baseTime = timeGetTime();
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_videoQueue.Pop();
			if (tag.size > 0)
			{
				DWORD dwMS = timeGetTime() - baseTime;
				INT offset = tag.samplePTS - dwMS;
				TRACE("delay:%d\n", offset);
				Sleep(offset < 0 ? 0 : offset);
				HDC hDC = GetDC(m_dx.GetHWND());
				if (hDC != NULL)
				{
					BITMAPINFO bmi = { 0 };
					bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmi.bmiHeader.biWidth = m_size.cx;
					bmi.bmiHeader.biHeight = -m_size.cy;
					bmi.bmiHeader.biPlanes = 1;
					bmi.bmiHeader.biBitCount = 32;
					bmi.bmiHeader.biCompression = BI_RGB;
					bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 4;
					BYTE* pvBits = NULL;
					HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
					if (hBitmap != NULL)
					{
						memcpy(pvBits, tag.bits, tag.size);
						TinyMemDC dc(hDC, hBitmap);
						TinyRectangle src = { 0,0,m_size.cx , m_size.cy };
						dc.Render(src, src, FALSE);
						SAFE_DELETE_OBJECT(hBitmap);
					}
					ReleaseDC(m_hWND, hDC);
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}

	void MRenderTask::OnScript(FLV_SCRIPTDATA* ps)
	{
		m_script = *ps;
	}

	void MRenderTask::OnAudio(BYTE* bits, LONG size, FLV_PACKET* packet)
	{
		if (packet->codeID == FLV_CODECID_AAC)
		{
			switch (packet->packetType)
			{
			case FLV_AudioSpecificConfig:
			{
				if (!m_aac->Open(bits, size, packet->bitsPerSample == 0 ? 8 : 16))
				{
					m_reader.Close(INFINITE);
				}
			}
			break;
			case FLV_AACRaw:
			{
				if (!m_bInitializeA)
				{
					m_bInitializeA = TRUE;
					if (!m_splayer.SetFormat(&m_aac->GetFormat(), size * 2))
					{
						m_reader.Close(INFINITE);
					}
				}
			}
			break;
			}
		}
	}

	void MRenderTask::OnVideo(BYTE* bits, LONG size, FLV_PACKET* packet)
	{
		if (packet->codeID == FLV_CODECID_H264)
		{
			switch (packet->packetType)
			{
			case FLV_AVCDecoderConfigurationRecord:
			{
				do
				{
					TinySize size1((LONG)m_script.width, (LONG)m_script.height);
					if (!m_h264->Initialize(size1, m_size))
					{
						m_reader.Close(INFINITE);
						break;
					}
					if (!m_h264->Open(bits, size))
					{
						m_reader.Close(INFINITE);
						break;
					}
				} while (0);
			}
			break;
			case FLV_NALU:
			{
				SampleTag tag = { 0 };
				tag.bits = bits;
				tag.size = size;
				tag.sampleDTS = packet->dts;
				tag.samplePTS = packet->pts;
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_h264->Decode(tag, bo, so))
				{
					tag.samplePTS = m_h264->GetYUV420()->pkt_pts;
					tag.sampleDTS = tag.samplePTS;
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					m_videoQueue.Push(tag);
					TRACE("Queue Size:%d\n", m_videoQueue.GetCount());
				}
			}
			break;
			}
		}
	}
}