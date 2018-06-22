#include "../stdafx.h"
#include "TinyVisualCommon.h"

namespace TinyFramework
{
	namespace Windowless
	{
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
		{
			return (LONG)MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
		}
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
		{
			return (LONG)MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
		}
		LONG DXtoHimetricX(LONG dx, LONG xPerInch)
		{
			return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
		}
		LONG DYtoHimetricY(LONG dy, LONG yPerInch)
		{
			return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
		}

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


		const TinyString TinyVisualPropertyConst::ORIENTATION = TEXT("orientation");

		const TinyString TinyVisualPropertyConst::PADDING = TEXT("padding");
		const TinyString TinyVisualPropertyConst::MARGIN = TEXT("margin");

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

		const TinyString TinyVisualPropertyConst::HORIZONTALALIGNMENT = TEXT("horizontal-alignment");
		const TinyString TinyVisualPropertyConst::VERTICALALIGNMENT = TEXT("vertical-alignment");
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
		const TinyString TinyVisualTag::WINDOW = TEXT("window");
		const TinyString TinyVisualTag::CONTEXT = TEXT("context");
		const TinyString TinyVisualTag::IMAGE = TEXT("image");
		const TinyString TinyVisualTag::ADD = TEXT("add");
		const TinyString TinyVisualTag::SYSCAPTION = TEXT("syscaption");
		const TinyString TinyVisualTag::BUTTON = TEXT("button");
		const TinyString TinyVisualTag::PANEL = TEXT("panel");
		const TinyString TinyVisualTag::STACKPANEL = TEXT("stackpanel");
		const TinyString TinyVisualTag::LIST = TEXT("list");
		const TinyString TinyVisualTag::LISTITEM = TEXT("listitem");
		const TinyString TinyVisualTag::LABEL = TEXT("label");
		const TinyString TinyVisualTag::ANIMATION = TEXT("animation");
		const TinyString TinyVisualTag::TAB = TEXT("tab");
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
		IMPLEMENT_DYNAMIC(TinyVisualDC, TinyHandleHDC);
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
		TinyVisualProperty::~TinyVisualProperty()
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

		TinyVisualVariant::TinyVisualVariant()
			:m_type(VARIANT_TYPE_EMPTY),
			m_value(NULL),
			m_size(0)
		{

		}
		TinyVisualVariant::~TinyVisualVariant()
		{
			Release();
		}
		TinyVisualVariant::TinyVisualVariant(const TinyVisualVariant& s)
		{
			this->m_value = malloc(s.m_size);
			if (this->m_value != NULL)
			{
				this->m_size = s.m_size;
				this->m_type = s.m_type;
				memcpy_s(this->m_value, this->m_size, s.m_value, this->m_size);
			}
		}
		TinyVisualVariant& TinyVisualVariant::operator=(const TinyVisualVariant& s)
		{
			if (&s != this)
			{
				Release();
				this->m_value = malloc(s.m_size);
				if (this->m_value != NULL)
				{
					this->m_size = s.m_size;
					this->m_type = s.m_type;
					memcpy_s(this->m_value, this->m_size, s.m_value, this->m_size);
				}
			}
			return *this;
		}
		TinyVisualVariant::TinyVisualVariant(TinyVisualVariant&& s)
			:m_size(s.m_size),
			m_type(s.m_type),
			m_value(s.m_value)
		{
			s.m_value = NULL;
			s.m_type = VARIANT_TYPE_EMPTY;
			s.m_size = 0;
		}
		BOOL TinyVisualVariant::IsEmpty() const
		{
			return VARIANT_TYPE_EMPTY || m_value == NULL;
		}
		BOOL TinyVisualVariant::GetBool() const
		{
			return *((BOOL*)this->m_value);
		}
		INT32 TinyVisualVariant::GetINT32() const
		{
			return *((INT32*)this->m_value);
		}
		UINT TinyVisualVariant::GetUINT() const
		{
			return *((UINT*)this->m_value);
		}
		DOUBLE	TinyVisualVariant::GetDouble() const
		{
			return *((DOUBLE*)this->m_value);
		}
		FLOAT	TinyVisualVariant::GetFloat() const
		{
			return *((FLOAT*)this->m_value);
		}
		POINT TinyVisualVariant::GetPoint() const
		{
			return *((POINT*)this->m_value);
		}
		SIZE TinyVisualVariant::GetSize() const
		{
			return *((SIZE*)this->m_value);
		}
		RECT TinyVisualVariant::GetRect() const
		{
			return *((RECT*)this->m_value);
		}
		HFONT TinyVisualVariant::GetFONT() const
		{
			return ((HFONT)this->m_value);
		}
		HRGN TinyVisualVariant::GetRGN() const
		{
			return ((HRGN)this->m_value);
		}
		const CHAR* TinyVisualVariant::GetString() const
		{
			return ((CHAR*)this->m_value);
		}
		COLORREF TinyVisualVariant::GetColor() const
		{
			return *((COLORREF*)this->m_value);
		}
		TinyImage* TinyVisualVariant::GetImage()
		{
			return ((TinyImage*)this->m_value);
		}
		BOOL TinyVisualVariant::SetPoint(const POINT& s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(POINT));
				if (this->m_value != NULL)
				{
					this->m_size = sizeof(POINT);
					this->m_type = VARIANT_TYPE_POINT;
					memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
					Release();
					return TRUE;
				}
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetSize(const SIZE& s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(SIZE));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(SIZE);
				this->m_type = VARIANT_TYPE_SIZE;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetRect(const RECT& s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(RECT));
				if (this->m_value != NULL)
				{
					this->m_size = sizeof(RECT);
					this->m_type = VARIANT_TYPE_RECT;
					memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
					Release();
					return TRUE;
				}
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetFONT(HFONT s)
		{
			if (!s)
				return FALSE;
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(HFONT));
				if (this->m_value != NULL)
				{
					this->m_size = sizeof(HFONT);
					this->m_type = VARIANT_TYPE_FONT;
					memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
					Release();
				}
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetRGN(HRGN s)
		{
			if (!s)
				return FALSE;
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(HRGN));
				if (this->m_value != NULL)
				{
					this->m_size = sizeof(HRGN);
					this->m_type = VARIANT_TYPE_REGION;
					memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
					Release();
				}
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetString(const string& s)
		{
			if (this->m_value != NULL)
			{
				if (m_size == s.size())
				{
					memcpy_s(this->m_value, this->m_size, (void*)&s[0], this->m_size);
					return TRUE;
				}
			}
			this->m_value = malloc(sizeof(CHAR)*(s.size() + 1));
			if (!this->m_value)
				return FALSE;
			this->m_size = s.size();
			this->m_type = VARIANT_TYPE_STRING;
			memcpy_s(this->m_value, this->m_size, &s[0], this->m_size);
			CHAR* val = (CHAR*)this->m_value;
			val[s.size()] = '\0';
			Release();
			return TRUE;
		}
		BOOL TinyVisualVariant::SetString(const TinyString& s)
		{
			return SetString(s.CSTR());
		}
		BOOL TinyVisualVariant::SetString(const CHAR* s)
		{
			if (!s)
				return FALSE;
			string val(s);
			return SetString(s);
		}
		BOOL TinyVisualVariant::SetColor(COLORREF s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(COLORREF));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(COLORREF);
				this->m_type = VARIANT_TYPE_COLOR;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetImage(TinyImage&& s)
		{
			this->m_value = malloc(sizeof(TinyImage));
			if (!this->m_value)
				return FALSE;
			this->m_size = sizeof(TinyImage);
			this->m_type = VARIANT_TYPE_IMAGE;
			Release();
#pragma push_macro("new")
#undef new
			::new(m_value) TinyPlaceNew<TinyImage>(std::move(s));
#pragma pop_macro("new")
			return TRUE;
		}
		BOOL TinyVisualVariant::SetBool(BOOL s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(BOOL));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(BOOL);
				this->m_type = VARIANT_TYPE_BOOL;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetINT32(INT32 s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(INT32));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(INT32);
				this->m_type = VARIANT_TYPE_INT32;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetUINT(UINT s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(UINT));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(UINT);
				this->m_type = VARIANT_TYPE_UINT;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetDouble(DOUBLE s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(DOUBLE));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(DOUBLE);
				this->m_type = VARIANT_TYPE_DOUBLE;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		BOOL TinyVisualVariant::SetFloat(FLOAT s)
		{
			if (this->m_value != NULL)
			{
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
			}
			else
			{
				this->m_value = malloc(sizeof(FLOAT));
				if (!this->m_value)
					return FALSE;
				this->m_size = sizeof(FLOAT);
				this->m_type = VARIANT_TYPE_FLOAT;
				memcpy_s(this->m_value, this->m_size, (void*)&s, this->m_size);
				Release();
			}
			return TRUE;
		}
		void TinyVisualVariant::Release()
		{
			if (this->m_type == VARIANT_TYPE_IMAGE)
			{

			}
			SAFE_FREE(this->m_value);
			this->m_size = 0;
			this->m_type = VARIANT_TYPE_EMPTY;
		}
	}
}
