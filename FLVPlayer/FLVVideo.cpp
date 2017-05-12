#include "stdafx.h"
#include "FLVVideo.h"

namespace FLVPlayer
{
	FLVVideo::FLVVideo(HWND hWND)
		:m_hWND(hWND),
		m_sPTS(0)
	{
		m_h264.Reset(new H264Decode());
		m_close.CreateEvent();
	}

	FLVVideo::~FLVVideo()
	{
	}
	BOOL FLVVideo::Submit()
	{
		ASSERT(m_h264);
		if (m_reader.Open("D:\\1.flv"))
		{
			return TinyTaskBase::Submit(BindCallback(&FLVVideo::OnMessagePump, this));
		}
		return FALSE;
	}
	BOOL FLVVideo::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		return TRUE;
	}
	void FLVVideo::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			m_timer.BeginTime();
			FLV_BLOCK block = { 0 };
			if (!m_reader.ReadBlock(block))
			{
				break;
			}
			if (block.type == FLV_SCRIPT)
			{
				m_size.cx = static_cast<LONG>(block.script.width);
				m_size.cy = static_cast<LONG>(block.script.height);
				m_rate = static_cast<DWORD>(block.script.framerate);
				continue;
			}
			if (block.type == FLV_AUDIO)
			{
				SAFE_DELETE_ARRAY(block.audio.data);
				SAFE_DELETE_ARRAY(block.video.data);
				continue;
			}
			if (block.video.codeID == FLV_CODECID_H264)
			{
				if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
				{
					if (!m_h264->Initialize(m_size, m_size))
						break;
					if (!m_h264->Open(block.video.data, block.video.size))
						break;
				}
				if (block.video.packetType == FLV_NALU)
				{
					SampleTag av = { 0 };
					av.bits = block.video.data;
					av.size = block.video.size;
					av.sampleDTS = block.dts;
					av.samplePTS = block.pts;
					BYTE* bo = NULL;
					LONG  so = 0;
					if (m_h264->Decode(av, bo, so))
					{
						this->OnRender(bo, so);
						m_timer.EndTime();
						Sleep(64);
						/*DWORD ms = m_timer.GetMillisconds();
						AVFrame* yuv420 = m_h264->GetYUV420();
						if (yuv420->pkt_pts > 0)
						{
							DWORD s = yuv420->pkt_pts - ms;
							if (m_sPTS != s)
							{
								INT offset = s - m_sPTS;
								m_sPTS = s;
								Sleep(offset < 0 ? 0 : offset);
							}
						}*/
					}
				}
			}
			SAFE_DELETE_ARRAY(block.audio.data);
			SAFE_DELETE_ARRAY(block.video.data);
		}
		m_reader.Close();
	}
	void FLVVideo::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_size.cx * m_size.cy * 3);
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_size.cx;
			bmi.bmiHeader.biHeight = -m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 3;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap)
			{
				memcpy(pvBits, bits, size);
				TinyMemDC dc(hDC, hBitmap);
				TinyRectangle src = { 0,0,m_size.cx,m_size.cy };
				dc.Render(src, src, FALSE);
				SAFE_DELETE_OBJECT(hBitmap);
			}
			ReleaseDC(m_hWND, hDC);
		}
	}
}