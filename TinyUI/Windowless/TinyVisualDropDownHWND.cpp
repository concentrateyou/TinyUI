#include "../stdafx.h"
#include "TinyVisualDropDownHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"
#include "TinyVisualVLayout.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualDropDownHWND::TinyVisualDropDownHWND()
		{

		}
		TinyVisualDropDownHWND::~TinyVisualDropDownHWND()
		{

		}
		DWORD TinyVisualDropDownHWND::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP & ~WS_CAPTION);
		}
		DWORD TinyVisualDropDownHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
		}
		LPCSTR TinyVisualDropDownHWND::RetrieveClassName()
		{
			return TEXT("TinyVisualDropDownHWND");
		}
		LPCSTR TinyVisualDropDownHWND::RetrieveTitle()
		{
			return TEXT("TinyVisualDropDownHWND");
		}
		HICON TinyVisualDropDownHWND::RetrieveIcon()
		{
			return NULL;
		}
		BOOL TinyVisualDropDownHWND::SetPosition(const TinyPoint& pos, const TinySize& size)
		{
			BOOL bRes = ::SetWindowPos(m_hWND, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, SWP_SHOWWINDOW);
			::SetActiveWindow(m_hWND);
			return bRes;
		}
		LRESULT TinyVisualDropDownHWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnCreate(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualDropDownHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return HTCLIENT;
		}

		LRESULT TinyVisualDropDownHWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnDestory(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualDropDownHWND::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				ShowWindow(SW_HIDE);
			}
			return FALSE;
		}
	}
}
