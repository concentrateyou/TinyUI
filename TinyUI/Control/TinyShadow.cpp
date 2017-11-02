#include "../stdafx.h"
#include "TinyShadow.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyShadow, TinyControl);

	TinyShadow::TinyShadow()
	{

	}
	TinyShadow::~TinyShadow()
	{

	}
	LPCSTR TinyShadow::RetrieveClassName()
	{
		return TEXT("TinyShadow");
	}
	LPCSTR TinyShadow::RetrieveTitle()
	{
		return TEXT("TinyShadow");
	}
	DWORD TinyShadow::RetrieveStyle()
	{
		return WS_POPUPWINDOW | WS_VISIBLE;
	}
	DWORD TinyShadow::RetrieveExStyle()
	{
		return WS_EX_TRANSPARENT | WS_EX_LAYERED;
	}

	BOOL TinyShadow::Create(HWND hParent, HWND hOwner)
	{
		return FALSE;
	}

	LRESULT TinyShadow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps = { 0 };
		HDC hDC = BeginPaint(m_hWND, &ps);
		Draw(hDC);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}

	LRESULT TinyShadow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetLayeredWindowAttributes(m_hWND, RGB(255, 255, 255), 255, LWA_ALPHA | LWA_COLORKEY);
		return FALSE;
	}
	void TinyShadow::Draw(HDC hDC)
	{

	}
}