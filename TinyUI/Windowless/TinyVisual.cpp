#include "../stdafx.h"
#include "TinyVisual.h"
#include "TinyVisualDocument.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisual, TinyObject);

		TinyVisual::TinyVisual()
			:m_spvisParent(NULL),
			m_spvisNext(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL),
			m_hrgnClip(NULL),
			m_hFONT(NULL),
			m_document(NULL),
			m_visible(TRUE),
			m_enable(TRUE),
			m_textAlign(0),
			m_textColor(RGB(255, 255, 255)),
			m_borderThickness(-1),
			m_borderStyle(PS_SOLID),
			m_backgroundImage(NULL),
			m_dwCount(0)
		{
			LOGFONT lf;
			::GetObject(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)), sizeof(LOGFONT), &lf);
			lf.lfCharSet = GB2312_CHARSET;
			m_hFONT = CreateFontIndirect(&lf);
		}
		TinyVisual::TinyVisual(TinyVisual* spvisParent, TinyVisualDocument* document)
			:m_spvisParent(spvisParent),
			m_spvisNext(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL),
			m_hrgnClip(NULL),
			m_hFONT(NULL),
			m_document(document),
			m_visible(TRUE),
			m_enable(TRUE),
			m_textAlign(0),
			m_textColor(RGB(255, 255, 255)),
			m_borderThickness(-1),
			m_borderStyle(-1),
			m_dwCount(0),
			m_backgroundImage(NULL)
		{
			LOGFONT lf;
			::GetObject(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)), sizeof(LOGFONT), &lf);
			lf.lfCharSet = GB2312_CHARSET;
			m_hFONT = CreateFontIndirect(&lf);
		}
		TinyVisual::~TinyVisual()
		{
			SAFE_DELETE_OBJECT(m_hFONT);
		}
		HWND TinyVisual::Handle() const
		{
			return m_document != NULL ? m_document->Handle() : NULL;
		}
		void TinyVisual::SetText(const TinyString& pzText)
		{
			m_szText = std::move(pzText);
		}
		TinyString	TinyVisual::GetText() const
		{
			return m_szText;
		}
		TinyString TinyVisual::RetrieveTag() const
		{
			return TinyString();
		}
		void TinyVisual::SetName(const TinyString& pzName)
		{
			m_szName = std::move(pzName);
		}
		TinyString	TinyVisual::GetName() const
		{
			return m_szName;
		}
		void TinyVisual::SetToolTip(const TinyString& pzTitle)
		{
			m_szToolTip = std::move(pzTitle);
		}
		TinyString	TinyVisual::GetToolTip() const
		{
			return m_szToolTip;
		}
		void TinyVisual::SetMaximumSize(const TinySize& size)
		{
			if (m_maximumSize != size)
			{
				m_maximumSize = size;
			}
		}
		void TinyVisual::SetMinimumSize(const TinySize& size)
		{
			if (m_minimumSize != size)
			{
				m_minimumSize = size;
			}
		}
		TinySize TinyVisual::GetMaximumSize() const
		{
			return m_maximumSize;
		}
		TinySize TinyVisual::GetMinimumSize() const
		{
			return m_minimumSize;
		}
		TinyImage*	TinyVisual::GetBackgroundImage()
		{
			return m_backgroundImage;
		}
		TinyColor TinyVisual::GetBackgroundColor() const
		{
			return m_backgroundColor;
		}
		TinyRectangle TinyVisual::GetBackgroundCenter() const
		{
			return m_backgroundCenter;
		}
		TinySize TinyVisual::GetBackgroundSize() const
		{
			return m_backgroundSize;
		}
		TinyPoint TinyVisual::GetBackgroundPosition() const
		{
			return m_backgroundPosition;
		}
		TinyColor TinyVisual::GetBorderColor() const
		{
			return m_borderColor;
		}
		INT	TinyVisual::GetBorderThickness() const
		{
			return m_borderThickness;
		}
		INT	TinyVisual::GetBorderStyle() const
		{
			return m_borderStyle;
		}
		BOOL TinyVisual::IsVisible() const
		{
			return m_visible;
		}
		BOOL TinyVisual::IsEnable() const
		{
			return m_enable;
		}
		BOOL TinyVisual::IsCapture() const
		{
			return m_document->GetCapture() == this;
		}
		BOOL TinyVisual::IsFocus() const
		{
			return m_document->GetFocus() == this;
		}
		BOOL TinyVisual::IsActive() const
		{
			return m_document->GetActive() == this;
		}
		void TinyVisual::SetVisible(BOOL visible)
		{
			if (m_visible != visible)
			{
				m_visible = visible;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetVisible(visible);
				}
			}
		}
		void TinyVisual::SetEnable(BOOL enable)
		{
			if (m_enable != enable)
			{
				m_enable = enable;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetEnable(enable);
				}
			}
		}
		void TinyVisual::SetTextColor(COLORREF color)
		{
			if (m_textColor != color)
			{
				m_textColor = color;
			}
		}
		void TinyVisual::SetTextAlian(UINT align)
		{
			if (m_textAlign != align)
			{
				m_textAlign = align;
			}
		}
		void TinyVisual::SetBackgroundImage(const TinyString& szName)
		{
			m_backgroundImage = TinyVisualResource::GetInstance()[szName];
		}
		void TinyVisual::SetBackgroundImage(TinyImage* image)
		{
			m_backgroundImage = image;
			if (m_backgroundImage != NULL)
			{
				TinyVisualResource::GetInstance().Add(m_backgroundImage);
			}
		}
		void TinyVisual::SetBackgroundColor(COLORREF color)
		{
			m_backgroundColor = color;
		}
		void TinyVisual::SetBackgroundCenter(const TinyRectangle& center)
		{
			m_backgroundCenter = center;
		}
		void TinyVisual::SetBackgroundSize(const TinySize& size)
		{
			m_backgroundSize = size;
		}
		void TinyVisual::SetBackgroundPosition(const TinyPoint& pos)
		{
			m_backgroundPosition = pos;
		}
		void TinyVisual::SetBorderColor(COLORREF color)
		{
			m_borderColor = color;
		}
		void TinyVisual::SetBorderThickness(INT cx)
		{
			m_borderThickness = cx;
		}
		void TinyVisual::SetBorderStyle(INT style)
		{
			m_borderStyle = style;
		}
		TinyPoint TinyVisual::GetPosition() const
		{
			return *((TinyPoint*)&m_rectangle);
		}
		void TinyVisual::SetPosition(const TinyPoint& newPos)
		{
			TinyPoint& pos = *((TinyPoint*)&m_rectangle);
			if (pos != newPos)
			{
				m_rectangle.SetPosition(newPos);
			}
		}
		TinySize TinyVisual::GetSize() const
		{
			return m_rectangle.Size();
		}
		void TinyVisual::SetSize(const TinySize& newsize)
		{
			TinySize size = *((TinySize*)&m_rectangle + 1);
			if (size != newsize)
			{
				m_rectangle.SetSize(newsize);
			}
		}
		TinyRectangle TinyVisual::GetRectangle() const
		{
			return m_rectangle;
		}
		TinyRectangle TinyVisual::GetWindowRect()const
		{
			ASSERT(m_document);
			return m_document->GetWindowRect(this);
		}
		TinyRectangle TinyVisual::GetClientRect() const
		{
			TinyRectangle s = m_rectangle;
			s.OffsetRect(-s.left, -s.top);
			return s;
		}
		TinyVisualDocument*	TinyVisual::GetDocument()
		{
			return m_document;
		}
		DWORD TinyVisual::GetChildCount() const
		{
			return m_dwCount;
		}
		void TinyVisual::SetClip(HRGN hrgnClip)
		{
			this->m_hrgnClip = hrgnClip;
		}
		void TinyVisual::SetFont(HFONT hFONT)
		{
			this->m_hFONT = hFONT;
		}
		HRGN TinyVisual::GetClip() const
		{
			return m_hrgnClip;
		}
		HFONT TinyVisual::GetFont() const
		{
			return m_hFONT;
		}
		HRESULT	TinyVisual::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
		{
			ASSERT(m_document->Handle());
			return ::SendMessage(m_document->Handle(), uMsg, wParam, lParam);
		}
		BOOL TinyVisual::Invalidate()
		{
			ASSERT(m_document);
			TinyRectangle s = GetWindowRect();
			return m_document->Invalidate(&s);
		}
		void TinyVisual::OnPosChange(const TinyPoint& oldpos, const TinyPoint& newpos)
		{

		}
		void TinyVisual::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{

		}
		HRESULT	TinyVisual::OnCreate()
		{
			EVENT_CREATE(EventArgs());
			return TRUE;
		}
		HRESULT	TinyVisual::OnDestory()
		{
			EVENT_DESTORY(EventArgs());
			return TRUE;
		}
		HRESULT	TinyVisual::OnInitialize()
		{
			EVENT_INITIALIZE(EventArgs());
			return TRUE;
		}
		BOOL	TinyVisual::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			MouseButtons button = NONE;
			if (dwFlags & MK_LBUTTON) button = MouseButtons::LBUTTON;
			if (dwFlags & MK_RBUTTON) button = MouseButtons::RBUTTON;
			if (dwFlags & MK_MBUTTON) button = MouseButtons::MBUTTON;
			EVENT_MOUSEMOVE(MouseEventArgs(button, 0, pos.x, pos.y, 0));
			return FALSE;
		}
		HRESULT TinyVisual::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			MouseButtons button = NONE;
			if (dwFlags & MK_LBUTTON) button = MouseButtons::LBUTTON;
			if (dwFlags & MK_RBUTTON) button = MouseButtons::RBUTTON;
			if (dwFlags & MK_MBUTTON) button = MouseButtons::MBUTTON;
			EVENT_MOUSEMOVE(MouseEventArgs(button, 0, pos.x, pos.y, zDelta));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseEnter()
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseLeave()
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDOWN(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MOUSECLICK(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDOWN(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MOUSECLICK(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDOWN(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KEYDOWN(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KEYUP(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_CHAR(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnFocus(BOOL bFlag)
		{
			EVENT_FOCUS(FocusEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnCapture(BOOL bFlag)
		{
			EVENT_CAPTURE(CaptureEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnActive(BOOL bFlag)
		{
			EVENT_ACTIVE(ActiveEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	 TinyVisual::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			return FALSE;
		}
		TinyString TinyVisual::GetProperty(const TinyString& name)
		{
			auto val = m_propertys.GetValue(name);
			if (val != NULL)
			{
				return *val;
			}
			return TinyString();
		}
		BOOL TinyVisual::IsProperty(const TinyString& name)
		{
			return m_propertys.Contain(name);
		}
		BOOL TinyVisual::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::NAME.STR()) == 0)
			{
				this->SetName(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXT.STR()) == 0)
			{
				this->SetText(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDIMAGE.STR()) == 0)
			{
				this->SetBackgroundImage(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDCOLOR.STR()) == 0)
			{
				this->SetBackgroundColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDCENTER.STR()) == 0)
			{
				this->SetBackgroundCenter(TinyVisualBuilder::GetRectangle(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDSIZE.STR()) == 0)
			{
				this->SetBackgroundSize(TinyVisualBuilder::GetSize(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDPOSITION.STR()) == 0)
			{
				this->SetBackgroundPosition(TinyVisualBuilder::GetPosition(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BORDERCOLOR.STR()) == 0)
			{
				this->SetBorderColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BORDERWIDTH.STR()) == 0)
			{
				this->SetBorderThickness(TinyVisualBuilder::GetInt32(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::BORDERSTYLE.STR()) == 0)
			{
				INT val = PS_SOLID;
				if (strcasecmp(value.STR(), "solid") == 0)
					val = PS_SOLID;
				if (strcasecmp(value.STR(), "dash") == 0)
					val = PS_DASH;
				if (strcasecmp(value.STR(), "dot") == 0)
					val = PS_DOT;
				this->SetBorderStyle(val);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TOOLTIP.STR()) == 0)
			{
				this->SetToolTip(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ENABLE.STR()) == 0)
			{
				this->SetEnable(TinyVisualBuilder::GetBool(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::VISIBLE.STR()) == 0)
			{
				this->SetVisible(TinyVisualBuilder::GetBool(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::POSITION.STR()) == 0)
			{
				this->SetPosition(TinyVisualBuilder::GetPosition(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SIZE.STR()) == 0)
			{
				this->SetSize(TinyVisualBuilder::GetSize(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MAXSIZE.STR()) == 0)
			{
				TinySize maxsize = TinyVisualBuilder::GetSize(value);
				this->SetMaximumSize(maxsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXTCOLOR.STR()) == 0)
			{
				this->SetTextColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXTALIGN.STR()) == 0)
			{
				this->SetTextAlian(TinyVisualBuilder::GetAlign(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTFAMILY.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				strcpy_s(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTWEIGHT.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				lf.lfWeight = atoi(value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTSIZE.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				TinySize size = TinyVisualBuilder::GetSize(value);
				lf.lfWidth = size.cx;
				lf.lfHeight = size.cy;
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTSTYLE.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				lf.lfItalic = FALSE;
				if (strcasecmp(value.STR(), "italic") == 0)
					lf.lfItalic = TRUE;
				if (strcasecmp(value.STR(), "oblique") == 0)
					lf.lfItalic = TRUE;
				if (strcasecmp(value.STR(), "normal") == 0)
					lf.lfItalic = FALSE;
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXTDECORATION.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfUnderline = FALSE;
				lf.lfStrikeOut = FALSE;
				if (strcasecmp(value.STR(), "underline") == 0)
					lf.lfUnderline = TRUE;
				if (strcasecmp(value.STR(), "overline") == 0)
					lf.lfStrikeOut = TRUE;
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			return FALSE;
		}
	}
}
