#include "stdafx.h"
#include "FLVTask.h"


FLVTask::FLVTask(HWND hWND)
	:m_hWND(hWND),
	m_queue(m_lock)
{
	m_audioDone.Reset(new Delegate<void(BYTE*, LONG, FLV_PARAM&)>(this, &FLVTask::OnAudioDone));
	m_videoDone.Reset(new Delegate<void(BYTE*, LONG, FLV_PARAM&)>(this, &FLVTask::OnVideoDone));
}


FLVTask::~FLVTask()
{
}


BOOL FLVTask::Submit()
{
	m_audio.CreateEvent();
	m_video.CreateEvent();
	return TinyTaskBase::Submit(BindCallback(&FLVTask::OnMessagePump, this));
}
BOOL FLVTask::Close(DWORD dwMS)
{
	if (m_audioTask)
	{
		m_audioTask->Close(INFINITE);
	}
	return TinyTaskBase::Close(dwMS);
}
void FLVTask::OnMessagePump()
{
	m_flv.EVENT_AUDIO += m_audioDone;
	m_flv.EVENT_VIDEO += m_videoDone;
	m_flv.Open("D:\\test.flv");
	m_flv.Parse();
	m_flv.Close();
	m_flv.EVENT_AUDIO -= m_audioDone;
	m_flv.EVENT_VIDEO -= m_videoDone;
}

LONGLONG lastAudioTimestamp = 0;
void FLVTask::OnAudioDone(BYTE* bits, LONG size, FLV_PARAM& val)
{
	if (bits == NULL)
	{
		m_queue.Initialize(1024 * 64);
		WAVEFORMATEX s = *reinterpret_cast<WAVEFORMATEX*>(val.param);
		m_audioTask.Reset(new FLVAudioTask(this, m_hWND, &s));
		m_audioTask->Submit();
	}
	else
	{
		m_queue.Write(bits, size);
	}
}

LONGLONG lastVideoTimestamp = 0;
void FLVTask::OnVideoDone(BYTE* bits, LONG size, FLV_PARAM& val)
{
	m_timer.BeginTime();
	HDC hDC = GetDC(m_hWND);
	if (hDC != NULL)
	{
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = val.videosize.cx;
		bmi.bmiHeader.biHeight = -val.videosize.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = val.videosize.cx * val.videosize.cy * 3;
		ASSERT(size == bmi.bmiHeader.biSizeImage);
		BYTE* pvBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
		if (hBitmap)
		{
			memcpy(pvBits, bits, size);
			TinyMemDC dc(hDC, hBitmap);
			TinyRectangle src = { 0,0,val.videosize.cx ,val.videosize.cy };
			dc.Render(src, src, FALSE);
			SAFE_DELETE_OBJECT(hBitmap);
		}
		ReleaseDC(m_hWND, hDC);
	}
	m_timer.EndTime();
	m_video.Lock(val.timestamp - lastVideoTimestamp - m_timer.GetMillisconds());
	lastVideoTimestamp = val.timestamp;
}
