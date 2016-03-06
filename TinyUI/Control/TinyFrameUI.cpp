#include "../stdafx.h"
#include "TinyFrameUI.h"

namespace TinyUI
{
	TinyFrameUI::TinyFrameUI()
	{

	}

	TinyFrameUI::~TinyFrameUI()
	{
	}

	LRESULT TinyFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);//退出应用程序
		return FALSE;
	}

	BOOL TinyFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}

	LRESULT TinyFrameUI::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (static_cast<BOOL>(wParam))
		{
			NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			RECT rect = ps->rgrc[0];
			ps->rgrc[0].top -= GetSystemMetrics(SM_CYCAPTION);
			bHandled = TRUE;
		}
		return FALSE;
	}

	DWORD TinyFrameUI::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW);
	}

	DWORD TinyFrameUI::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR TinyFrameUI::RetrieveClassName()
	{
		return TEXT("FramwUI");
	}

	LPCSTR TinyFrameUI::RetrieveTitle()
	{
		return TEXT("FramwUI");
	}

	HICON TinyFrameUI::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT TinyFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;


		return TRUE;
	}

	LRESULT TinyFrameUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		return FALSE;
	}

	LRESULT TinyFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinyFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinyFrameUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}
}

