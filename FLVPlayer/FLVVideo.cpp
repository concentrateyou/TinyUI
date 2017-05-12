#include "stdafx.h"
#include "FLVVideo.h"

namespace FLVPlayer
{
	FLVVideo::FLVVideo(HWND hWND)
		:m_hWND(hWND)
	{
		m_h264.Reset(new H264Decode());
		m_close.CreateEvent();

		m_render.Reset(new FLVRender(*this));
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
		m_render->Submit();
		for (;;)
		{
			if (m_close.Lock(0))
				break;

			if (m_queueSize > 5)
				continue;

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
					{
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
						break;
					}
					if (!m_h264->Open(block.video.data, block.video.size))
					{
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
						break;
					}
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
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
						av.bits = new BYTE[so];
						memcpy(av.bits, bo, so);
						av.size = so;
						av.samplePTS = m_h264->GetYUV420()->pkt_pts;
						TinyAutoLock lock(m_lock);
						m_queue.push(av);
						m_queueSize++;
					}
					else
					{
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
				}
			}
		}
		m_reader.Close();
	}
	//////////////////////////////////////////////////////////////////////////
	FLVRender::FLVRender(FLVVideo& video)
		:m_video(video),
		m_currentPTS(0)
	{

	}
	BOOL FLVRender::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVRender::OnMessagePump, this));
	}
	void FLVRender::OnMessagePump()
	{
		for (;;)
		{
			m_video.m_lock.Lock();
			if (m_video.m_queueSize > 0)
			{
				SampleTag tag = m_video.m_queue.front();
				m_video.m_queue.pop();
				m_video.m_queueSize--;
				m_video.m_lock.Unlock();
				m_timer.BeginTime();
				OnRender(tag.bits, tag.size);
				SAFE_DELETE_ARRAY(tag.bits);
				m_timer.EndTime();
				if (m_currentPTS != tag.samplePTS)
				{
					DWORD ms = m_timer.GetMillisconds();
					if (ms > 40)
					{
						TRACE("ms:%d\n", ms);
					}
					INT offset = tag.samplePTS - m_currentPTS - ms;
					if (offset < 0)
					{
						INT a = 0;
					}
					Sleep(offset < 0 ? 0 : offset);
					m_currentPTS = tag.samplePTS;
				}
			}
			else
			{
				m_video.m_lock.Unlock();
			}
		}
	}
	BOOL FLVRender::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVRender::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_video.m_size.cx * m_video.m_size.cy * 3);
		HDC hDC = GetDC(m_video.m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_video.m_size.cx;
			bmi.bmiHeader.biHeight = -m_video.m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_video.m_size.cx * m_video.m_size.cy * 3;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap)
			{
				memcpy(pvBits, bits, size);
				TinyMemDC dc(hDC, hBitmap);
				TinyRectangle src = { 0,0,m_video.m_size.cx,m_video.m_size.cy };
				dc.Render(src, src, FALSE);
				SAFE_DELETE_OBJECT(hBitmap);
			}
			ReleaseDC(m_video.m_hWND, hDC);
		}
	}
}