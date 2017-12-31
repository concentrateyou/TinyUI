#include "../stdafx.h"
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		const TinyString TinyVisualPropertyConst::NAME = TEXT("name");
		const TinyString TinyVisualPropertyConst::TEXT = TEXT("text");
		const TinyString TinyVisualPropertyConst::TOOLTIP = TEXT("tooltip");
		const TinyString TinyVisualPropertyConst::MAXSIZE = TEXT("maxsize");
		const TinyString TinyVisualPropertyConst::MINSIZE = TEXT("minsize");
		const TinyString TinyVisualPropertyConst::POSITION = TEXT("position");
		const TinyString TinyVisualPropertyConst::SIZE = TEXT("size");
		const TinyString TinyVisualPropertyConst::VISIBLE = TEXT("visible");
		const TinyString TinyVisualPropertyConst::ENABLE = TEXT("enable");
		const TinyString TinyVisualPropertyConst::CURSOR = TEXT("cursor");

		const TinyString TinyVisualPropertyConst::BACKGROUNDIMAGE = TEXT("background-image");
		const TinyString TinyVisualPropertyConst::BACKGROUNDCOLOR = TEXT("background-color");
		const TinyString TinyVisualPropertyConst::BACKGROUNDCENTER = TEXT("background-center");
		const TinyString TinyVisualPropertyConst::BACKGROUNDSIZE = TEXT("background-size");
		const TinyString TinyVisualPropertyConst::BACKGROUNDPOSITION = TEXT("background-position");

		const TinyString TinyVisualPropertyConst::BORDERWIDTH = TEXT("border-width");
		const TinyString TinyVisualPropertyConst::BORDERCOLOR = TEXT("border-color");
		const TinyString TinyVisualPropertyConst::BORDERSTYLE = TEXT("border-style");
		const TinyString TinyVisualPropertyConst::BORDERIMAGE = TEXT("border-image");
		const TinyString TinyVisualPropertyConst::BORDERCENTER = TEXT("border-center");

		const TinyString TinyVisualPropertyConst::FONTFAMILY = TEXT("font-family");
		const TinyString TinyVisualPropertyConst::FONTSIZE = TEXT("font-size");
		const TinyString TinyVisualPropertyConst::FONTWEIGHT = TEXT("font-weight");
		const TinyString TinyVisualPropertyConst::FONTSTYLE = TEXT("font-style");

		const TinyString TinyVisualPropertyConst::TEXTCOLOR = TEXT("text-color");
		const TinyString TinyVisualPropertyConst::TEXTALIGN = TEXT("text-align");
		const TinyString TinyVisualPropertyConst::TEXTDECORATION = TEXT("text-decoration");
		const TinyString TinyVisualPropertyConst::MULTILINE = TEXT("multiline");
		const TinyString TinyVisualPropertyConst::READONLY = TEXT("readonly");

		const TinyString TinyVisualPropertyConst::ALIGNMENT = TEXT("alignment");
		const TinyString TinyVisualPropertyConst::VALUE = TEXT("value");
		const TinyString TinyVisualPropertyConst::OPTIONHIGHLIGHT = TEXT("highlight");

		const TinyString TinyVisualPropertyConst::IMAGENORMAL = TEXT("image-normal");
		const TinyString TinyVisualPropertyConst::IMAGEHIGHLIGHT = TEXT("image-highlight");
		const TinyString TinyVisualPropertyConst::IMAGEDOWN = TEXT("image-down");
		const TinyString TinyVisualPropertyConst::IMAGEPUSH = TEXT("image-push");

		const TinyString TinyVisualPropertyConst::IMAGE = TEXT("image");
		const TinyString TinyVisualPropertyConst::IMAGELIST = TEXT("imagelist");

		const TinyString TinyVisualPropertyConst::ARROWNORMAL = TEXT("arrow-normal");
		const TinyString TinyVisualPropertyConst::ARROWDOWN = TEXT("arrow-down");
		const TinyString TinyVisualPropertyConst::ARROWHIGHLIGHT = TEXT("arrow-highlight");
		const TinyString TinyVisualPropertyConst::ARROWPUSH = TEXT("arrow-push");

		const TinyString TinyVisualPropertyConst::ARROW1NORMAL = TEXT("arrow1-normal");
		const TinyString TinyVisualPropertyConst::ARROW1HIGHLIGHT = TEXT("arrow1-highlight");
		const TinyString TinyVisualPropertyConst::ARROW1DOWN = TEXT("arrow1-down");

		const TinyString TinyVisualPropertyConst::ARROW2NORMAL = TEXT("arrow2-normal");
		const TinyString TinyVisualPropertyConst::ARROW2HIGHLIGHT = TEXT("arrow2-highlight");
		const TinyString TinyVisualPropertyConst::ARROW2DOWN = TEXT("arrow2-down");

		const TinyString TinyVisualPropertyConst::SCROLLBARGROOVE = TEXT("scrollbar-groove");
		const TinyString TinyVisualPropertyConst::SCROLLBARNORMAL = TEXT("scrollbar-normal");
		const TinyString TinyVisualPropertyConst::SCROLLBARHIGHLIGHT = TEXT("scrollbar-highlight");

		const TinyString TinyVisualPropertyConst::SHADOWIMAGE = TEXT("shadow-image");
		const TinyString TinyVisualPropertyConst::SHADOWBOX = TEXT("shadow-box");

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
		const TinyString TinyVisualTagConst::WINDOW = TEXT("window");
		const TinyString TinyVisualTagConst::CONTEXT = TEXT("context");
		const TinyString TinyVisualTagConst::IMAGE = TEXT("image");
		const TinyString TinyVisualTagConst::ADD = TEXT("add");
		const TinyString TinyVisualTagConst::SYSCAPTION = TEXT("syscaption");
		const TinyString TinyVisualTagConst::BUTTON = TEXT("button");
		const TinyString TinyVisualTagConst::PANEL = TEXT("panel");
		const TinyString TinyVisualTagConst::LIST = TEXT("list");
		const TinyString TinyVisualTagConst::LISTITEM = TEXT("listitem");
		const TinyString TinyVisualTagConst::LABEL = TEXT("label");
		const TinyString TinyVisualTagConst::ANIMATION = TEXT("animation");
		const TinyString TinyVisualTagConst::TAB = TEXT("tab");
		const TinyString TinyVisualTagConst::STACKPANEL = TEXT("stackpanel");
		const TinyString TinyVisualTagConst::HBOXLAYOUT = TEXT("hboxlayout");
		const TinyString TinyVisualTagConst::VBOXLAYOUT = TEXT("vboxlayout");
		const TinyString TinyVisualTagConst::LISTBOX = TEXT("listbox");
		const TinyString TinyVisualTagConst::TOOLBAR = TEXT("toolbar");
		const TinyString TinyVisualTagConst::TREEVIEW = TEXT("treeview");
		const TinyString TinyVisualTagConst::HSCROLLBAR = TEXT("hscrollbar");
		const TinyString TinyVisualTagConst::VSCROLLBAR = TEXT("vscrollbar");
		const TinyString TinyVisualTagConst::COMBOBOX = TEXT("combobox");
		const TinyString TinyVisualTagConst::OPTION = TEXT("option");
		const TinyString TinyVisualTagConst::TEXTBOX = TEXT("textbox");
		const TinyString TinyVisualTagConst::STRETCH = TEXT("stretch");
		const TinyString TinyVisualTagConst::NATIVEWINDOW = TEXT("nativewindow");
		const TinyString TinyVisualTagConst::MENUITEM = TEXT("menuitem");
		const TinyString TinyVisualTagConst::CONTEXTMENU = TEXT("contextmenu");
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
		//////////////////////////////////////////////////////////////////////////
		TinyVisualProperty::TinyVisualProperty(const TinyString& name, const TinyString& value)
			:m_name(std::move(name)),
			m_value(std::move(value))
		{

		}
		TinyVisualProperty::TinyVisualProperty(const TinyVisualProperty& other)
			: m_name(std::move(other.name())),
			m_value(std::move(other.value()))
		{

		}
		void TinyVisualProperty::operator=(const TinyVisualProperty& other)
		{
			m_name = std::move(other.name());
			m_value = std::move(other.value());
		}
		const TinyString& TinyVisualProperty::name() const
		{
			return m_name;
		}
		const TinyString& TinyVisualProperty::value() const
		{
			return m_value;
		}
	}
}
