#include "stdafx.h"
#include "RenderTask.h"
#include "MainFrame.h"

RenderTask::RenderTask(HWND hWND, DShow::VideoCapture* pVideo, const DShow::VideoCaptureParam& videoParam)
	:m_hWND(hWND),
	m_pVideo(pVideo),
	m_videoParam(videoParam)
{

}


RenderTask::~RenderTask()
{
}

BOOL RenderTask::Submit()
{
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	return TinyTaskBase::Submit(BindCallback(&RenderTask::OnMessagePump, this));
}
BOOL RenderTask::Close(DWORD dwMS)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMS);
}
void RenderTask::OnMessagePump()
{
	for (;;)
	{
		if (m_close.Lock(33))
			break;
		BYTE* bits = m_pVideo->GetPointer();
		if (bits)
		{
			RECT rectangle = { 0 };
			::GetWindowRect(m_hWND, &rectangle);
			TinyUI::TinyWindowDC wdc(m_hWND);
			TinySize size = m_videoParam.GetSize();
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = size.cx;
			bmi.bmiHeader.biHeight = size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = size.cx * size.cy * 4;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(wdc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			memcpy(pvBits, bits, bmi.bmiHeader.biSizeImage);
			TinyUI::TinyMemDC mdc(wdc, hBitmap);
			::BitBlt(wdc, 0, 0, TO_CX(rectangle), TO_CY(rectangle), mdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
		}
	}
}
void RenderTask::OnExit()
{
	m_close.SetEvent();
}
