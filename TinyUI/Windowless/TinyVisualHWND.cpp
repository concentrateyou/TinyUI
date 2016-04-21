#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualHWND::TinyVisualHWND()
			:m_vtree(NULL)
		{

		}
		TinyVisualHWND::~TinyVisualHWND()
		{

		}
		BOOL TinyVisualHWND::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
		{
			return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
		}
		DWORD TinyVisualHWND::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW);
		}
		DWORD TinyVisualHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
		}
		LPCSTR TinyVisualHWND::RetrieveClassName()
		{
			return TEXT("TinyVisualHWND");
		}
		LPCSTR TinyVisualHWND::RetrieveTitle()
		{
			return TEXT("FramwUI");
		}
		HICON TinyVisualHWND::RetrieveIcon()
		{
			return NULL;
		}
		BOOL TinyVisualHWND::Initialize()
		{
			if (m_vtree = new TinyVisualTree(this))
			{
				return m_vtree->Initialize();
			}
			return FALSE;
		}
		void TinyVisualHWND::Uninitialize()
		{
			if (!m_vtree) return;
			m_vtree->Uninitialize();
			SAFE_DELETE(m_vtree);
		}

		LRESULT TinyVisualHWND::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			PAINTSTRUCT ps = { 0 };
			HDC hDC = BeginPaint(m_hWND, &ps);
			EndPaint(m_hWND, &ps);
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			m_size.cx = LOWORD(lParam);
			m_size.cy = HIWORD(lParam);
			return FALSE;
		}

		LRESULT TinyVisualHWND::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			Uninitialize();
			PostQuitMessage(0);//退出应用程序
			return FALSE;
		}

		LRESULT TinyVisualHWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			Initialize();
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			if (static_cast<BOOL>(wParam))
			{
				NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				ps->rgrc[0].left = ps->lppos->x;
				ps->rgrc[0].top = ps->lppos->y;
				ps->rgrc[0].bottom = ps->lppos->y + ps->lppos->cy;
				ps->rgrc[0].right = ps->lppos->x + ps->lppos->cx;
			}
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
	}
}
