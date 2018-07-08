#include "../stdafx.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualComboBoxHWND.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualComboBoxHWND, TinyVisualWindowless);

		TinyVisualComboBoxHWND::TinyVisualComboBoxHWND()
		{

		}

		TinyVisualComboBoxHWND::~TinyVisualComboBoxHWND()
		{

		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveClassName()
		{
			return TEXT("VISUALCOMBOBOXHWND");
		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveTitle()
		{
			return TEXT("VISUALCOMBOBOXHWND");
		}
		DWORD TinyVisualComboBoxHWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}

		DWORD TinyVisualComboBoxHWND::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}

		LRESULT	 TinyVisualComboBoxHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return HTCLIENT;
		}
		LRESULT	TinyVisualComboBoxHWND::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == FALSE)
			{
				if (IsWindow(m_hWND))
				{
					ShowWindow(SW_HIDE);
				}
			}
			return TinyVisualWindowless::OnActivate(uMsg, wParam, lParam, bHandled);;
		}
		LRESULT	 TinyVisualComboBoxHWND::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return TinyVisualWindowless::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}
		LRESULT	TinyVisualComboBoxHWND::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return TinyVisualWindowless::OnMouseHover(uMsg, wParam, lParam, bHandled);
		}
		LRESULT	TinyVisualComboBoxHWND::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return TinyVisualWindowless::OnMouseLeave(uMsg, wParam, lParam, bHandled);
		}
	}
}
