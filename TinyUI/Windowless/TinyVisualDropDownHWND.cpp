#include "../stdafx.h"
#include "TinyVisualDropDownHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

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
			return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW & ~WS_CAPTION);
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
			return TinyControl::OnDestory(uMsg, wParam, lParam, bHandled);
		}
	}
}
