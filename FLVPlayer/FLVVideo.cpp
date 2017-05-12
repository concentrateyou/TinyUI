#include "stdafx.h"
#include "FLVVideo.h"

namespace FLVPlayer
{
	FLVVideo::FLVVideo(HWND hWND)
		:m_hWND(hWND),
		m_currentPTS(0)
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
		if (m_reader.Open("D:\\3.flv"))
		{
			FLV_SCRIPTDATA script = m_reader.GetScript();
			m_rate = static_cast<DWORD>(script.framerate);
			m_size.cx = static_cast<LONG>(script.width);
			m_size.cy = static_cast<LONG>(script.height);
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
			if (block.type != FLV_VIDEO)
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
						int64_t pkt_pts = m_h264->GetYUV420()->pkt_pts;
						if (pkt_pts != m_currentPTS)
						{
							DWORD ms = m_timer.GetMillisconds();
							INT offset = pkt_pts - m_currentPTS - ms;
							TRACE("pkt_pts:%d, ms:%d offset:%d\n", (DWORD)pkt_pts, ms, offset);
							Sleep(offset < 0 ? 0 : offset);
							m_currentPTS = pkt_pts;
						}
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