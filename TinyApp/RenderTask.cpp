#include "stdafx.h"
#include "RenderTask.h"


RenderTask::RenderTask(BYTE*& bits, HWND hWND, const Media::VideoCaptureParam& param)
	:m_bits(bits),
	m_hWND(hWND),
	m_param(param)
{

}


RenderTask::~RenderTask()
{
}

BOOL RenderTask::Submit()
{
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	Closure s = BindCallback(&RenderTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
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
			break;;
		if (m_bits)
		{
			RECT rectangle = { 0 };
			::GetWindowRect(m_hWND, &rectangle);
			TinyUI::TinyWindowDC wdc(m_hWND);
			TinySize size = m_param.GetSize();
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = size.cx;
			bmi.bmiHeader.biHeight = size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = size.cx * size.cy * 3;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(wdc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			memcpy(pvBits, m_bits, bmi.bmiHeader.biSizeImage);
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
