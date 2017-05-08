#include "../stdafx.h"
#include "TinyVisualPopupHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualPopupHWND, TinyVisualHWND);
		TinyVisualPopupHWND::TinyVisualPopupHWND()
		{

		}
		TinyVisualPopupHWND::~TinyVisualPopupHWND()
		{

		}
		DWORD TinyVisualPopupHWND::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP & ~WS_CAPTION);
		}
		DWORD TinyVisualPopupHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
		}
		LPCSTR TinyVisualPopupHWND::RetrieveClassName()
		{
			return TEXT("TinyVisualPopupHWND");
		}
		LPCSTR TinyVisualPopupHWND::RetrieveTitle()
		{
			return TEXT("TinyVisualPopupHWND");
		}
		HICON TinyVisualPopupHWND::RetrieveIcon()
		{
			return NULL;
		}
		BOOL TinyVisualPopupHWND::SetPosition(const TinyPoint& pos, const TinySize& size)
		{
			BOOL bRes = ::SetWindowPos(m_hWND, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, SWP_SHOWWINDOW);
			::SetActiveWindow(m_hWND);
			return bRes;
		}
		LRESULT TinyVisualPopupHWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnCreate(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualPopupHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return HTCLIENT;
		}

		LRESULT TinyVisualPopupHWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnDestory(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualPopupHWND::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
