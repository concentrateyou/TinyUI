#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualShadow.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualShadow::TinyVisualShadow()
			:m_image(NULL)
		{

		}
		TinyVisualShadow::~TinyVisualShadow()
		{

		}

		LPCSTR TinyVisualShadow::RetrieveClassName()
		{
			return TEXT("TinyVisualShadow");
		}

		LPCSTR TinyVisualShadow::RetrieveTitle()
		{
			return TEXT("TinyVisualShadow");
		}

		HICON TinyVisualShadow::RetrieveIcon()
		{
			return NULL;
		}

		DWORD TinyVisualShadow::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX);
		}

		DWORD TinyVisualShadow::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		}

		LRESULT TinyVisualShadow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualShadow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}

		LRESULT TinyVisualShadow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return TRUE;
		}
		LRESULT TinyVisualShadow::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			if (static_cast<BOOL>(wParam))
			{
				NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				ps->rgrc[0].left = ps->lppos->x < 0 ? 0 : ps->lppos->x;
				ps->rgrc[0].top = ps->lppos->y < 0 ? 0 : ps->lppos->y;
				ps->rgrc[0].bottom = ps->rgrc[0].top + ps->lppos->cy;
				ps->rgrc[0].right = ps->rgrc[0].left + ps->lppos->cx;
			}
			return TRUE;
		}
		BOOL TinyVisualShadow::SetShadow(TinyImage* image)
		{
			if (image != NULL && !image->IsEmpty())
			{
				m_image = image;
				return TRUE;
			}
			return FALSE;
		}
		TinyRectangle TinyVisualShadow::GetShadowBox() const
		{
			return m_box;
		}
		void TinyVisualShadow::SetShadowBox(const TinyRectangle& box)
		{
			m_box = box;
		}
		BOOL TinyVisualShadow::DrawShadow()
		{
			if (!m_hWND || !m_image || m_image->IsEmpty())
				return FALSE;
			TinyRectangle windowRect;
			::GetWindowRect(m_hWND, &windowRect);
			TinyWindowDC windowDC(m_hWND);
			HDC hMemDC = ::CreateCompatibleDC(windowDC);
			if (!hMemDC)
				return FALSE;
			HBITMAP hBitmap = ::CreateCompatibleBitmap(windowDC, windowRect.Width(), windowRect.Height());
			if (!hBitmap)
			{
				if (hMemDC != NULL)
				{
					::DeleteDC(hMemDC);
					hMemDC = NULL;
				}
				return FALSE;
			}
			HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			TinyMemDC memDC(hMemDC, m_image->GetHBITMAP(0));
			TinyRectangle dst;
			dst.SetRect(0, 0, windowRect.Width(), windowRect.Height());
			TinyRectangle dstCenter = dst;
			dstCenter.left += m_box.left;
			dstCenter.top += m_box.top;
			dstCenter.right -= m_box.right;
			dstCenter.bottom -= m_box.bottom;
			TinyRectangle src = m_image->GetRectangle();
			TinyRectangle srcCenter = src;
			srcCenter.left += m_box.left;
			srcCenter.top += m_box.top;
			srcCenter.right -= m_box.right;
			srcCenter.bottom -= m_box.bottom;
			memDC.Render(dst, dstCenter, src, srcCenter, TRUE);
			TinyPoint pos;
			TinyPoint dstPos = windowRect.Position();
			TinySize  dstSize = windowRect.Size();
			BOOL bRes = ::UpdateLayeredWindow(m_hWND, windowDC, &dstPos, &dstSize, hMemDC, &pos, 0, &bs, 2);
			::SelectObject(hMemDC, hOldBitmap);
			::DeleteDC(hMemDC);
			::DeleteObject(hBitmap);
			hMemDC = NULL;
			hBitmap = NULL;
			hOldBitmap = NULL;
			return bRes;
		}
	}
}
