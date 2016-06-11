#include "../stdafx.h"
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		const TinyString TinyVisualProperty::NAME = TEXT("name");
		const TinyString TinyVisualProperty::TEXT = TEXT("text");
		const TinyString TinyVisualProperty::TOOLTIP = TEXT("tooltip");
		const TinyString TinyVisualProperty::MAXSIZE = TEXT("maxsize");
		const TinyString TinyVisualProperty::MINSIZE = TEXT("minsize");
		const TinyString TinyVisualProperty::SIZE = TEXT("size");
		const TinyString TinyVisualProperty::VISIBLE = TEXT("visible");
		const TinyString TinyVisualProperty::ENABLE = TEXT("enable");
		const TinyString TinyVisualProperty::IMAGENORMAL = TEXT("image-normal");
		const TinyString TinyVisualProperty::IMAGEHIGHLIGHT = TEXT("image-highlight");
		const TinyString TinyVisualProperty::IMAGEDOWN = TEXT("image-down");
		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualTag::WINDOW = TEXT("window");
		const TinyString TinyVisualTag::CAPTION = TEXT("caption");
		const TinyString TinyVisualTag::SYSBUTTON = TEXT("sysbutton");
		//////////////////////////////////////////////////////////////////////////
		TinyVisualCacheDC::TinyVisualCacheDC(HWND hWND)
			:m_hWND(hWND),
			m_hPrev(NULL),
			m_hBitmap(NULL)
		{
			ASSERT(m_hWND);
			HDC hDC = ::GetDC(m_hWND);
			Attach(hDC);
			RECT s = { 0 };
			GetWindowRect(m_hWND, &s);
			SetSize(TO_CX(s), TO_CY(s));
		}
		TinyVisualCacheDC::~TinyVisualCacheDC()
		{
			ASSERT(m_hWND);
			if (m_hDC != NULL)
			{
				if (m_hPrev != NULL)
					::SelectObject(m_hDC, m_hPrev);
				if (m_hBitmap != NULL)
					SAFE_DELETE_OBJECT(m_hBitmap);
				HDC hDC = Detach();
				::ReleaseDC(m_hWND, hDC);
			}
		}
		void TinyVisualCacheDC::SetSize(INT cx, INT cy)
		{
			ASSERT(m_hDC);
			if (m_size.cx != cx || m_size.cy != cy)
			{
				m_size.cx = cx;
				m_size.cy = cy;
				if (m_hPrev != NULL)
					::SelectObject(m_hDC, m_hPrev);
				SAFE_DELETE_OBJECT(m_hBitmap);
				m_hBitmap = ::CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
				m_hPrev = ::SelectObject(m_hDC, m_hBitmap);
			}
		}
	}
}
