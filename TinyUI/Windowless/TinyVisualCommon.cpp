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
		const TinyString TinyVisualProperty::CURSOR = TEXT("cursor");

		const TinyString TinyVisualProperty::BACKGROUNDIMAGE = TEXT("background-image");
		const TinyString TinyVisualProperty::BACKGROUNDCOLOR = TEXT("background-color");
		const TinyString TinyVisualProperty::BACKGROUNDCENTER = TEXT("background-center");
		const TinyString TinyVisualProperty::BACKGROUNDSIZE = TEXT("background-size");
		const TinyString TinyVisualProperty::BACKGROUNDPOSITION = TEXT("background-position");

		const TinyString TinyVisualProperty::BORDERWIDTH = TEXT("border-width");
		const TinyString TinyVisualProperty::BORDERCOLOR = TEXT("border-color");
		const TinyString TinyVisualProperty::BORDERSTYLE = TEXT("border-style");
		const TinyString TinyVisualProperty::BORDERIMAGE = TEXT("border-image");
		const TinyString TinyVisualProperty::BORDERCENTER = TEXT("border-center");

		const TinyString TinyVisualProperty::FONTFAMILY = TEXT("font-family");
		const TinyString TinyVisualProperty::FONTSIZE = TEXT("font-size");
		const TinyString TinyVisualProperty::FONTWEIGHT = TEXT("font-weight");
		const TinyString TinyVisualProperty::FONTSTYLE = TEXT("font-style");

		const TinyString TinyVisualProperty::TEXTCOLOR = TEXT("text-color");
		const TinyString TinyVisualProperty::TEXTALIGN = TEXT("text-align");
		const TinyString TinyVisualProperty::TEXTDECORATION = TEXT("text-decoration");
		const TinyString TinyVisualProperty::MULTILINE = TEXT("multiline");
		const TinyString TinyVisualProperty::READONLY = TEXT("readonly");

		const TinyString TinyVisualProperty::ALIGNMENT = TEXT("alignment");
		const TinyString TinyVisualProperty::VALUE = TEXT("value");
		const TinyString TinyVisualProperty::OPTIONHIGHLIGHT = TEXT("highlight");

		const TinyString TinyVisualProperty::IMAGENORMAL = TEXT("image-normal");
		const TinyString TinyVisualProperty::IMAGEHIGHLIGHT = TEXT("image-highlight");
		const TinyString TinyVisualProperty::IMAGEDOWN = TEXT("image-down");
		const TinyString TinyVisualProperty::IMAGEPUSH = TEXT("image-push");

		const TinyString TinyVisualProperty::IMAGE = TEXT("image");

		const TinyString TinyVisualProperty::ARROWNORMAL = TEXT("arrow-normal");
		const TinyString TinyVisualProperty::ARROWDOWN = TEXT("arrow-down");
		const TinyString TinyVisualProperty::ARROWHIGHLIGHT = TEXT("arrow-highlight");
		const TinyString TinyVisualProperty::ARROWPUSH = TEXT("arrow-push");

		const TinyString TinyVisualProperty::ARROW1NORMAL = TEXT("arrow1-normal");
		const TinyString TinyVisualProperty::ARROW1HIGHLIGHT = TEXT("arrow1-highlight");
		const TinyString TinyVisualProperty::ARROW1DOWN = TEXT("arrow1-down");

		const TinyString TinyVisualProperty::ARROW2NORMAL = TEXT("arrow2-normal");
		const TinyString TinyVisualProperty::ARROW2HIGHLIGHT = TEXT("arrow2-highlight");
		const TinyString TinyVisualProperty::ARROW2DOWN = TEXT("arrow2-down");

		const TinyString TinyVisualProperty::SCROLLBARGROOVE = TEXT("scrollbar-groove");
		const TinyString TinyVisualProperty::SCROLLBARNORMAL = TEXT("scrollbar-normal");
		const TinyString TinyVisualProperty::SCROLLBARHIGHLIGHT = TEXT("scrollbar-highlight");

		const TinyString TinyVisualProperty::SHADOWIMAGE = TEXT("shadow-image");
		const TinyString TinyVisualProperty::SHADOWBOX = TEXT("shadow-box");

		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualCursor::AUTO = TEXT("auto");
		const TinyString TinyVisualCursor::HAND = TEXT("hand");
		const TinyString TinyVisualCursor::TEXT = TEXT("text");
		const TinyString TinyVisualCursor::MOVE = TEXT("move");
		const TinyString TinyVisualCursor::POINTER = TEXT("pointer");
		const TinyString TinyVisualCursor::WAIT = TEXT("wait");
		const TinyString TinyVisualCursor::HELP = TEXT("help");;
		const TinyString TinyVisualCursor::ERESIZE = TEXT("e-resize");;
		const TinyString TinyVisualCursor::NERESIZE = TEXT("ne-resize");
		const TinyString TinyVisualCursor::NWRESIZE = TEXT("nw-resize");
		const TinyString TinyVisualCursor::NRESIZE = TEXT("n-resize");
		const TinyString TinyVisualCursor::SERESIZE = TEXT("se-resize");
		const TinyString TinyVisualCursor::SWRESIZE = TEXT("sw-resize");
		const TinyString TinyVisualCursor::SRESIZE = TEXT("s-resize");
		const TinyString TinyVisualCursor::WRESIZE = TEXT("w-resize");
		LPSTR CursorFromString(TinyString& str)
		{
			if (str == TinyVisualCursor::HAND)
			{
				return IDC_HAND;
			}
			if (str == TinyVisualCursor::WAIT)
			{
				return IDC_WAIT;
			}
			if (str == TinyVisualCursor::HELP)
			{
				return IDC_HELP;
			}
			if (str == TinyVisualCursor::MOVE)
			{
				return IDC_CROSS;
			}
			if (str == TinyVisualCursor::ERESIZE)
			{
				return IDC_SIZEWE;
			}
			if (str == TinyVisualCursor::WRESIZE)
			{
				return IDC_SIZEWE;
			}
			if (str == TinyVisualCursor::SRESIZE)
			{
				return IDC_SIZENS;
			}
			if (str == TinyVisualCursor::NRESIZE)
			{
				return IDC_SIZENS;
			}
			if (str == TinyVisualCursor::NERESIZE)
			{
				return IDC_SIZENESW;
			}
			if (str == TinyVisualCursor::NWRESIZE)
			{
				return IDC_SIZENWSE;
			}
			if (str == TinyVisualCursor::SERESIZE)
			{
				return IDC_SIZENWSE;
			}
			if (str == TinyVisualCursor::SWRESIZE)
			{
				return IDC_SIZENESW;
			}
			return IDC_ARROW;
		}
		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualTag::WINDOW = TEXT("window");
		const TinyString TinyVisualTag::CONTEXT = TEXT("context");
		const TinyString TinyVisualTag::IMAGE = TEXT("image");
		const TinyString TinyVisualTag::ADD = TEXT("add");
		const TinyString TinyVisualTag::SYSCAPTION = TEXT("syscaption");
		const TinyString TinyVisualTag::BUTTON = TEXT("button");
		const TinyString TinyVisualTag::PANEL = TEXT("panel");
		const TinyString TinyVisualTag::LIST = TEXT("list");
		const TinyString TinyVisualTag::LISTITEM = TEXT("listitem");
		const TinyString TinyVisualTag::LABEL = TEXT("label");
		const TinyString TinyVisualTag::ANIMATION = TEXT("animation");
		const TinyString TinyVisualTag::TAB = TEXT("tab");
		const TinyString TinyVisualTag::STACKPANEL = TEXT("stackpanel");
		const TinyString TinyVisualTag::HBOXLAYOUT = TEXT("hboxlayout");
		const TinyString TinyVisualTag::VBOXLAYOUT = TEXT("vboxlayout");
		const TinyString TinyVisualTag::LISTBOX = TEXT("listbox");
		const TinyString TinyVisualTag::TOOLBAR = TEXT("toolbar");
		const TinyString TinyVisualTag::TREEVIEW = TEXT("treeview");
		const TinyString TinyVisualTag::HSCROLLBAR = TEXT("hscrollbar");
		const TinyString TinyVisualTag::VSCROLLBAR = TEXT("vscrollbar");
		const TinyString TinyVisualTag::COMBOBOX = TEXT("combobox");
		const TinyString TinyVisualTag::OPTION = TEXT("option");
		const TinyString TinyVisualTag::TEXTBOX = TEXT("textbox");
		const TinyString TinyVisualTag::STRETCH = TEXT("stretch");
		const TinyString TinyVisualTag::NATIVEWINDOW = TEXT("nativewindow");
		const TinyString TinyVisualTag::MENUITEM = TEXT("menuitem");
		const TinyString TinyVisualTag::CONTEXTMENU = TEXT("contextmenu");
		//////////////////////////////////////////////////////////////////////////
		TinyVisualDC::TinyVisualDC(HWND hWND)
			:m_hWND(hWND),
			m_hMemDC(NULL),
			m_hBitmap(NULL),
			m_hOldBitmap(NULL)
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
				m_hMemDC = ::CreateCompatibleDC(m_hDC);
				m_hBitmap = ::CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
				m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
			}
		}
		HDC	 TinyVisualDC::GetMemDC() const
		{
			return m_hMemDC;
		}
		BOOL TinyVisualDC::Render(const RECT& s)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, s.left, s.top, SRCCOPY);
		}
		BOOL TinyVisualDC::Render(const RECT& s, INT x, INT y)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, x, y, SRCCOPY);
		}
		BOOL TinyVisualDC::RenderLayer(const RECT& s)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			if (::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, s.left, s.top, SRCCOPY))
			{
				HDC hScreenDC = GetDC(NULL);
				BLENDFUNCTION blendFunction;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;
				blendFunction.BlendFlags = 0;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.SourceConstantAlpha = 255;
				RECT wndRect;
				::GetWindowRect(m_hWND, &wndRect);
				POINT ptSrc = { 0,0 };
				SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
				UpdateLayeredWindow(m_hWND, hScreenDC, &ptSrc, &wndSize, m_hMemDC, &ptSrc, 0, &blendFunction, 2);
				ReleaseDC(NULL, hScreenDC);
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyVisualDC::RenderLayer(const RECT& s, INT x, INT y)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			if (::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, x, y, SRCCOPY))
			{
				HDC hScreenDC = GetDC(NULL);
				BLENDFUNCTION blendFunction;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;
				blendFunction.BlendFlags = 0;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.SourceConstantAlpha = 255;
				RECT wndRect;
				::GetWindowRect(m_hWND, &wndRect);
				POINT ptSrc = { 0,0 };
				SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
				UpdateLayeredWindow(m_hWND, hScreenDC, &ptSrc, &wndSize, m_hMemDC, &ptSrc, 0, &blendFunction, 2);
				ReleaseDC(NULL, hScreenDC);
				return TRUE;
			}
			return FALSE;
		}
	}
}
