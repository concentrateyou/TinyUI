#include "stdafx.h"
#include "FLVFrame.h"


FLVFrame::FLVFrame()
{
}


FLVFrame::~FLVFrame()
{
}

LRESULT FLVFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL FLVFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD FLVFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD FLVFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR FLVFrame::RetrieveClassName()
{
	return TEXT("FLVFrame");
}

LPCSTR FLVFrame::RetrieveTitle()
{
	return TEXT("FLVFrame");
}

HICON FLVFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT FLVFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_audioDone.Reset(new Delegate<void(BYTE*, LONG, FLV_PARAM&)>(this, &FLVFrame::OnAudioDone));
	m_videoDone.Reset(new Delegate<void(BYTE*, LONG, FLV_PARAM&)>(this, &FLVFrame::OnVideoDone));
	m_flv.EVENT_AUDIO += m_audioDone;
	m_flv.EVENT_VIDEO += m_videoDone;
	m_flv.Open("D:\\test.flv");
	m_flv.Parse();
	return FALSE;
}

LRESULT FLVFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_flv.EVENT_AUDIO -= m_audioDone;
	m_flv.EVENT_VIDEO -= m_videoDone;
	m_flv.Close();
	return FALSE;
}

LRESULT FLVFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT s = { 0 };
	HDC hDC = BeginPaint(m_hWND, &s);

	EndPaint(m_hWND, &s);
	return FALSE;
}

LRESULT FLVFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void FLVFrame::OnAudioDone(BYTE* bits, LONG size, FLV_PARAM& val)
{
	if (bits == NULL)
	{
		WAVEFORMATEX s = *reinterpret_cast<WAVEFORMATEX*>(val.param);
		m_player.Initialize(m_hWND, &s);
	}
	else
	{
		TRACE("audio:%d\n", val.timestamp);
		m_player.Play(bits, size);
	}
}

void FLVFrame::OnVideoDone(BYTE* bits, LONG size, FLV_PARAM& val)
{
	TRACE("video:%d\n", val.timestamp);
}