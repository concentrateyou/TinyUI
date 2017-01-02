#include "../stdafx.h"
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		const TinyString TinyVisualConst::LEFT = "left";
		const TinyString TinyVisualConst::TOP = "top";
		const TinyString TinyVisualConst::RIGHT = "right";
		const TinyString TinyVisualConst::BOTTOM = "bottom";
		const TinyString TinyVisualConst::CENTER = "center";
		const TinyString TinyVisualConst::STRETCH = "stretch";
		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualProperty::NAME = TEXT("name");
		const TinyString TinyVisualProperty::TEXT = TEXT("text");
		const TinyString TinyVisualProperty::TOOLTIP = TEXT("tooltip");
		const TinyString TinyVisualProperty::MAXSIZE = TEXT("maxsize");
		const TinyString TinyVisualProperty::MINSIZE = TEXT("minsize");
		const TinyString TinyVisualProperty::POSITION = TEXT("position");
		const TinyString TinyVisualProperty::SIZE = TEXT("size");
		const TinyString TinyVisualProperty::VISIBLE = TEXT("visible");
		const TinyString TinyVisualProperty::ENABLE = TEXT("enable");
		const TinyString TinyVisualProperty::IMAGENORMAL = TEXT("image-normal");
		const TinyString TinyVisualProperty::IMAGEHIGHLIGHT = TEXT("image-highlight");
		const TinyString TinyVisualProperty::IMAGEDOWN = TEXT("image-down");
		const TinyString TinyVisualProperty::HORIZONTALALIGNMENT = TEXT("horizontal-alignment");
		const TinyString TinyVisualProperty::VERTICALALIGNMENT = TEXT("vertical-alignment");
		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualTag::WINDOW = TEXT("window");
		const TinyString TinyVisualTag::SYSCAPTION = TEXT("syscaption");
		const TinyString TinyVisualTag::BUTTON = TEXT("button");
		const TinyString TinyVisualTag::LABEL = TEXT("label");
		const TinyString TinyVisualTag::TAB = TEXT("tab");
		const TinyString TinyVisualTag::STACKPANEL = TEXT("stackpanel");
		const TinyString TinyVisualTag::HORIZONTALLAYOUT = TEXT("horizontallayout");
		const TinyString TinyVisualTag::VERTICALLAYOUT = TEXT("verticallayout");
		const TinyString TinyVisualTag::LISTBOX = TEXT("listbox");
		const TinyString TinyVisualTag::TOOLBAR = TEXT("toolbar");
		const TinyString TinyVisualTag::TREEVIEW = TEXT("treeview");
		const TinyString TinyVisualTag::HSCROLLBAR = TEXT("hscrollbar");
		const TinyString TinyVisualTag::VSCROLLBAR = TEXT("vscrollbar");
		const TinyString TinyVisualTag::COMBOBOX = TEXT("combobox");
		const TinyString TinyVisualTag::RICHTEXT = TEXT("richtext");
		//////////////////////////////////////////////////////////////////////////
		TinyVisualDC::TinyVisualDC(HWND hWND)
			:m_hWND(hWND),
			m_hMemDC(NULL),
			m_hMemBitmap(NULL),
			m_OldBitmap(NULL)
		{
			ASSERT(m_hWND);
			HDC hDC = ::GetDC(m_hWND);
			Attach(hDC);
			SetGraphicsMode(hDC, GM_ADVANCED);
			SetBkMode(hDC, TRANSPARENT);
			RECT s = { 0 };
			GetWindowRect(m_hWND, &s);
			SetSize(TO_CX(s), TO_CY(s));
		}
		TinyVisualDC::~TinyVisualDC()
		{
			ASSERT(m_hWND);
			if (m_hDC != NULL)
			{
				SAFE_DELETE_OBJECT(m_hMemBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_OldBitmap != NULL)
					{
						SelectObject(m_hMemDC, m_OldBitmap);
						m_OldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				HDC hDC = Detach();
				::ReleaseDC(m_hWND, hDC);
			}
		}
		void TinyVisualDC::SetSize(INT cx, INT cy)
		{
			ASSERT(m_hDC);
			if (m_size.cx != cx || m_size.cy != cy)
			{
				m_size.cx = cx;
				m_size.cy = cy;
				SAFE_DELETE_OBJECT(m_hMemBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_OldBitmap != NULL)
					{
						SelectObject(m_hMemDC, m_OldBitmap);
						m_OldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				m_hMemDC = ::CreateCompatibleDC(m_hDC);
				m_hMemBitmap = ::CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
				m_OldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hMemBitmap);
			}
		}
		HDC	 TinyVisualDC::GetMemDC() const
		{
			return m_hMemDC;
		}
		BOOL TinyVisualDC::Render(const RECT& s)
		{
			if (!m_hMemDC || !m_hMemBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, s.left, s.top, SRCCOPY);
		}
		BOOL TinyVisualDC::Render(const RECT& s, INT x, INT y)
		{
			if (!m_hMemDC || !m_hMemBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, x, y, SRCCOPY);
		}
	}
}
