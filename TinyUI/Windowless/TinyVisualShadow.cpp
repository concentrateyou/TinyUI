#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualShadow.h"

namespace TinyUI
{
	namespace Windowless
	{
		ShadowDC::ShadowDC(HWND hWND)
			:m_hWND(hWND),
			m_hMemDC(NULL),
			m_hBitmap(NULL),
			m_hOldBitmap(NULL)
		{
			ASSERT(m_hWND);
			HDC hDC = ::GetWindowDC(m_hWND);
			Attach(hDC);
			SetGraphicsMode(hDC, GM_ADVANCED);
			SetBkMode(hDC, TRANSPARENT);
		}
		ShadowDC::~ShadowDC()
		{
			ASSERT(m_hWND);
			if (m_hDC != NULL)
			{
				SAFE_DELETE_OBJECT(m_hBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_hOldBitmap != NULL)
					{
						SelectObject(m_hMemDC, m_hOldBitmap);
						m_hOldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				HDC hDC = Detach();
				::ReleaseDC(m_hWND, hDC);
			}
		}
		void ShadowDC::BeginDraw(const TinySize& size)
		{
			BeginDraw(size.cx, size.cy);
		}
		void ShadowDC::BeginDraw(INT cx, INT cy)
		{
			ASSERT(m_hDC);
			if (m_size.cx != cx || m_size.cy != cy)
			{
				m_size.cx = cx;
				m_size.cy = cy;
				SAFE_DELETE_OBJECT(m_hBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_hOldBitmap != NULL)
					{
						::SelectObject(m_hMemDC, m_hOldBitmap);
						m_hOldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				m_hMemDC = ::CreateCompatibleDC(m_hDC);
				if (m_hMemDC != NULL)
				{
					m_hBitmap = ::CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
				}
				if (m_hBitmap != NULL)
				{
					m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
				}
			}
		}
		void ShadowDC::EndDraw()
		{
			if (m_hMemDC != NULL && m_hOldBitmap != NULL)
			{
				::SelectObject(m_hMemDC, m_hOldBitmap);
			}
		}
		HDC	 ShadowDC::GetMemDC() const
		{
			return m_hMemDC;
		}
		//////////////////////////////////////////////////////////////////////////
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
			m_shadowDC.Reset(new ShadowDC(m_hWND));
			ASSERT(m_shadowDC);
			return FALSE;
		}
		LRESULT TinyVisualShadow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			m_shadowDC.Reset(NULL);
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
			if (!m_hWND || !m_shadowDC)
				return FALSE;
			if (!m_image || m_image->IsEmpty())
				return FALSE;
			TinyRectangle windowRect;
			::GetWindowRect(m_hWND, &windowRect);
			m_shadowDC->BeginDraw(windowRect.Size());
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			TinyMemDC memDC(m_shadowDC->GetMemDC(), m_image->GetHBITMAP(0));
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
			::UpdateLayeredWindow(m_hWND, m_shadowDC->Handle(), &dstPos, &dstSize, m_shadowDC->GetMemDC(), &pos, 0, &bs, 2);
			m_shadowDC->EndDraw();
			return TRUE;
		}
	}
}
