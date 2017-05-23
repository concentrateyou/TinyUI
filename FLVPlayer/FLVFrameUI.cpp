#include "stdafx.h"
#include "FLVFrameUI.h"

namespace FLVPlayer
{
	FLVFrameUI::FLVFrameUI()
	{
	}


	FLVFrameUI::~FLVFrameUI()
	{
	}

	LRESULT FLVFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}

	BOOL FLVFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD FLVFrameUI::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD FLVFrameUI::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR FLVFrameUI::RetrieveClassName()
	{
		return TEXT("FLVFrameUI");
	}

	LPCSTR FLVFrameUI::RetrieveTitle()
	{
		return TEXT("FLVFrameUI");
	}

	HICON FLVFrameUI::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT FLVFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_decode.Reset(new FLVDecode(m_hWND));
		m_decode->Submit();
		return FALSE;
	}

	LRESULT FLVFrameUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_decode)
			m_decode->Close(INFINITE);
		return FALSE;
	}

	LRESULT FLVFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT FLVFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}