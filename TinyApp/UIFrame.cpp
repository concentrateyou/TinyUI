#include "stdafx.h"
#include "UIFrame.h"


CUIFrame::CUIFrame()
{

}

CUIFrame::~CUIFrame()
{
}

LRESULT CUIFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL CUIFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD CUIFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD CUIFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR CUIFrame::RetrieveClassName()
{
	return TEXT("CUIFrame");
}

LPCSTR CUIFrame::RetrieveTitle()
{
	return TEXT("CUIFrame");
}

HICON CUIFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT CUIFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	m_videoCB = BindCallback(&CUIFrame::OnVideo, this);
	m_audioCB = BindCallback(&CUIFrame::OnAudio, this);


	m_onVideoStart.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnVideoStart));
	m_onVideoStop.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnVideoStop));
	m_videoStart.Create(m_hWND, 20, 50, 150, 23);
	m_videoStart.EVENT_Click += m_onVideoStart;
	m_videoStart.SetText("ø™ º ”∆µ≤∂ªÒ");
	m_videoStop.Create(m_hWND, 20, 80, 150, 23);
	m_videoStop.EVENT_Click += m_onVideoStop;
	m_videoStop.SetText("‘›Õ£ ”∆µ≤∂ªÒ");

	m_onAudioStart.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnAudioStart));
	m_onAudioStop.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnAudioStop));
	m_audioStart.Create(m_hWND, 400, 50, 150, 23);
	m_audioStart.EVENT_Click += m_onAudioStart;
	m_audioStart.SetText("ø™ º“Ù∆µ≤∂ªÒ");
	m_audioStop.Create(m_hWND, 400, 80, 150, 23);
	m_audioStop.EVENT_Click += m_onAudioStop;
	m_audioStop.SetText("‘›Õ£“Ù∆µ≤∂ªÒ");

	m_control.Create(m_hWND, 20, 120, 1024, 768);
	m_control.SetText("≤‚ ‘");

	m_videoDevice1.Create(m_hWND, 20, 20, 150, 23);
	m_onVideoChange1.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnVideoSelectChange1));
	m_videoDevice1.EVENT_SelectChange += m_onVideoChange1;
	m_videoDevice2.Create(m_hWND, 180, 20, 150, 23);
	m_onVideoChange2.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnVideoSelectChange2));
	m_videoDevice2.EVENT_SelectChange += m_onVideoChange2;
	m_videoNames.clear();
	Media::VideoCapture::GetDevices(m_videoNames);
	for (UINT i = 0; i < m_videoNames.size(); i++)
	{
		m_videoDevice1.AddString(m_videoNames[i].name().c_str());
	}

	m_audioDevice1.Create(m_hWND, 400, 20, 150, 23);
	m_onAudioChange1.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnAudioSelectChange1));
	m_audioDevice1.EVENT_SelectChange += m_onAudioChange1;
	m_audioDevice2.Create(m_hWND, 600, 20, 150, 23);
	m_onAudioChange2.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnAudioSelectChange2));
	m_audioDevice2.EVENT_SelectChange += m_onAudioChange2;
	m_audioNames.clear();
	Media::AudioCapture::GetDevices(m_audioNames);
	for (UINT i = 0; i < m_audioNames.size(); i++)
	{
		m_audioDevice1.AddString(m_audioNames[i].name().c_str());
	}


	return FALSE;
}

void CUIFrame::OnVideo(const BYTE* pBits, INT size, LPVOID lpData)
{
	AM_MEDIA_TYPE* pMediaType = static_cast<AM_MEDIA_TYPE*>(lpData);
	if (pMediaType)
	{
		RECT rectangle = { 0 };
		::GetWindowRect(m_control.Handle(), &rectangle);
		VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		TinyUI::TinyWindowDC wdc(m_control.Handle());
		BITMAPINFO bi = { 0 };
		bi.bmiHeader = h->bmiHeader;
		BYTE* pvBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(wdc, &bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
		memcpy(pvBits, pBits, size);
		TinyUI::TinyMemDC mdc(wdc, hBitmap);
		::BitBlt(wdc, 0, 0, TO_CX(rectangle), TO_CY(rectangle), mdc, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
	}
}
void CUIFrame::OnAudio(const BYTE*, INT, LPVOID)
{

}

LRESULT CUIFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_videoStart.EVENT_Click -= m_onVideoStart;
	m_videoStop.EVENT_Click -= m_onVideoStop;
	m_videoDevice1.EVENT_SelectChange -= m_onVideoChange1;
	m_videoDevice2.EVENT_SelectChange -= m_onVideoChange2;

	m_audioStart.EVENT_Click -= m_onAudioStart;
	m_audioStop.EVENT_Click -= m_onAudioStop;
	m_audioDevice1.EVENT_SelectChange -= m_onAudioChange1;
	m_audioDevice2.EVENT_SelectChange -= m_onAudioChange2;

	return FALSE;
}

LRESULT CUIFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT CUIFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void CUIFrame::OnVideoSelectChange1(INT index)
{
	m_videoParams.clear();
	m_videoDevice2.ResetContent();
	Media::VideoCapture::GetDeviceParams(m_videoNames[index], m_videoParams);
	for (UINT i = 0; i < m_videoParams.size(); i++)
	{
		m_videoDevice2.AddString(m_videoParams[i].ToString().c_str());
	}
}

void CUIFrame::OnVideoSelectChange2(INT index)
{
	const Media::VideoCaptureParam& param = m_videoParams[index];
	m_videoDevice.Uninitialize();
	m_videoDevice.Initialize(m_videoNames[m_videoDevice1.GetCurSel()], m_videoCB);
	m_videoDevice.Allocate(param);
}

void CUIFrame::OnAudioSelectChange1(INT index)
{
	m_audioParams.clear();
	m_audioDevice2.ResetContent();
	Media::AudioCapture::GetDeviceParams(m_audioNames[index], m_audioParams);
	for (UINT i = 0; i < m_audioParams.size(); i++)
	{
		m_audioDevice2.AddString(m_audioParams[i].ToString().c_str());
	}
}
void CUIFrame::OnAudioSelectChange2(INT index)
{
	const Media::AudioCaptureParam& param = m_audioParams[index];
	m_audioDevice.Uninitialize();
	m_audioDevice.Initialize(m_audioNames[m_audioDevice1.GetCurSel()], m_audioCB);
	m_audioDevice.Allocate(param);
}

void CUIFrame::OnVideoStart(void*, INT)
{
	m_videoDevice.Start();
}
void CUIFrame::OnVideoStop(void*, INT)
{
	m_videoDevice.Stop();
}

void CUIFrame::OnAudioStart(void*, INT)
{
	m_audioDevice.Start();
}
void CUIFrame::OnAudioStop(void*, INT)
{
	m_audioDevice.Stop();
}

