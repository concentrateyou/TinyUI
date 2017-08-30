#include "../stdafx.h"
#include "TinyVisualDropDownHWND.h"
#include "TinyVisualComboBox.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualDropDownHWND, TinyVisualHWND);
		TinyVisualDropDownHWND::TinyVisualDropDownHWND(TinyVisualComboBox* pOwner)
			:m_pOwner(pOwner),
			m_count(0)
		{

		}
		TinyVisualDropDownHWND::~TinyVisualDropDownHWND()
		{

		}
		DWORD TinyVisualDropDownHWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
		DWORD TinyVisualDropDownHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_LTRREADING);
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
		BOOL TinyVisualDropDownHWND::IsPopup()
		{
			return IsWindowVisible(m_hWND);
		}
		BOOL TinyVisualDropDownHWND::SetPosition(const TinyPoint& pos, const TinySize& size)
		{
			BOOL bRes = ::SetWindowPos(m_hWND, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
			::UpdateWindow(m_hWND);
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
				EVENT_ACTIVE(ActiveEventArgs(FALSE));
			}
			if (LOWORD(wParam) == WA_ACTIVE)
			{
				EVENT_ACTIVE(ActiveEventArgs(TRUE));
			}
			if (LOWORD(wParam) == WA_CLICKACTIVE)
			{
				EVENT_ACTIVE(ActiveEventArgs(TRUE));
			}
			return TinyVisualHWND::OnActivate(uMsg, wParam, lParam, bHandled);
		}
	}
}
