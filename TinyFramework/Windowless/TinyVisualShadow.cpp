#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualShadow.h"

namespace TinyFramework
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

		BOOL TinyVisualShadow::SetShadow(TinyImage* image)
		{
			if (image != NULL && !image->IsEmpty())
			{
				m_image = image;
				return TRUE;
			}
			return FALSE;
		}
		TinyRectangle TinyVisualShadow::GetShadowRectangle() const
		{
			return m_rectangle;
		}
		void TinyVisualShadow::SetShadowRectangle(const TinyRectangle& rectangle)
		{
			m_rectangle = rectangle;
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
			TinyMemDC memDC(hMemDC, m_image->GetHBITMAP());
			TinyRectangle dst;
			dst.SetRect(0, 0, windowRect.Width(), windowRect.Height());
			TinyRectangle dstCenter = dst;
			dstCenter.left += m_rectangle.left;
			dstCenter.top += m_rectangle.top;
			dstCenter.right -= m_rectangle.right;
			dstCenter.bottom -= m_rectangle.bottom;
			TinyRectangle src = m_image->GetRectangle();
			TinyRectangle srcCenter = src;
			srcCenter.left += m_rectangle.left;
			srcCenter.top += m_rectangle.top;
			srcCenter.right -= m_rectangle.right;
			srcCenter.bottom -= m_rectangle.bottom;
			memDC.Render(dst, dstCenter, src, srcCenter, TRUE);
			TinyPoint pos;
			TinyPoint dstPos = windowRect.Position();
			TinySize  dstSize = windowRect.Size();
			BOOL bRes = ::UpdateLayeredWindow(m_hWND, windowDC, &dstPos, &dstSize, hMemDC, &pos, 0, &bs, ULW_ALPHA);
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
