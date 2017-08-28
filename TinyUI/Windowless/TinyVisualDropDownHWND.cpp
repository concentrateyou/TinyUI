#include "../stdafx.h"
#include "TinyVisualDropDownHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualDropDownHWND, TinyVisualHWND);
		TinyVisualDropDownHWND::TinyVisualDropDownHWND()
		{

		}
		TinyVisualDropDownHWND::~TinyVisualDropDownHWND()
		{

		}
		DWORD TinyVisualDropDownHWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPSIBLINGS | WS_VISIBLE);
		}
		DWORD TinyVisualDropDownHWND::RetrieveExStyle()
		{
			return (WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_LAYERED);
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

		void TinyVisualDropDownHWND::OnInitialize()
		{
		}

		void TinyVisualDropDownHWND::OnUninitialize()
		{

		}

		BOOL TinyVisualDropDownHWND::SetPosition(const TinyPoint& pos, const TinySize& size)
		{
			UINT uFlags = SWP_SHOWWINDOW;
			if (size.IsEmpty())
			{
				uFlags |= SWP_NOSIZE;
			}
			if (pos.IsEmpty())
			{
				uFlags |= SWP_NOMOVE;
			}
			BOOL bRes = ::SetWindowPos(m_hWND, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, uFlags);
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
				//ShowWindow(SW_HIDE);
			}
			return FALSE;
		}
	}
}
