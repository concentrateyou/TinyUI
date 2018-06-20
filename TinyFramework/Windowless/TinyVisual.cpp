#include "../stdafx.h"
#include "TinyVisual.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
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
			m_borderImage(NULL),
			m_dwCount(0)
		{
			LOGFONT lf;
			::GetObject(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)), sizeof(LOGFONT), &lf);
			lf.lfCharSet = GB2312_CHARSET;
			m_hFONT = CreateFontIndirect(&lf);
			m_szCursor = TinyVisualCursor::AUTO;
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
			m_backgroundImage(NULL),
			m_borderImage(NULL),
			m_dwCount(0)
		{
			LOGFONT lf;
			::GetObject(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)), sizeof(LOGFONT), &lf);
			lf.lfCharSet = GB2312_CHARSET;
			m_hFONT = CreateFontIndirect(&lf);
			m_szCursor = TinyVisualCursor::AUTO;
		}
		TinyVisual::~TinyVisual()
		{
			SAFE_DELETE_OBJECT(m_hFONT);
		}
		HWND TinyVisual::Handle() const
		{
			return m_document != NULL ? m_document->Handle() : NULL;
		}
		void TinyVisual::SetText(const TinyString& szText)
		{
			m_szText = std::move(szText);
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
		void TinyVisual::SetCursor(const TinyString& szCursor)
		{
			m_szCursor = std::move(szCursor);
		}
		void TinyVisual::SetToolTip(const TinyString& szTitle)
		{
			m_szToolTip = std::move(szTitle);
		}
		TinyString	TinyVisual::GetToolTip() const
		{
			return m_szToolTip;
		}
		void TinyVisual::SetPadding(const TinyRectangle& padding)
		{
			if (m_padding != padding)
			{
				m_padding = padding;
			}
		}
		void TinyVisual::SetMargin(const TinyRectangle& margin)
		{
			if (m_margin != margin)
			{
				m_margin = margin;
			}
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
		UINT TinyVisual::GetBorderThickness() const
		{
			return m_borderThickness;
		}
		INT	TinyVisual::GetBorderStyle() const
		{
			return m_borderStyle;
		}
		TinyImage* TinyVisual::GetBorderImage()
		{
			return m_borderImage;
		}
		TinyRectangle TinyVisual::GetBorderCenter() const
		{
			return m_borderCenter;
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
		void TinyVisual::SetBorderThickness(UINT cx)
		{
			m_borderThickness = cx;
		}
		void TinyVisual::SetBorderStyle(INT style)
		{
			m_borderStyle = style;
		}
		void TinyVisual::SetBorderImage(const TinyString& szName)
		{
			m_borderImage = TinyVisualResource::GetInstance()[szName];
		}
		void TinyVisual::SetBorderImage(TinyImage* image)
		{
			m_borderImage = image;
			if (m_borderImage != NULL)
			{
				TinyVisualResource::GetInstance().Add(m_borderImage);
			}
		}
		void TinyVisual::SetBorderCenter(const TinyRectangle& center)
		{
			m_borderCenter = center;
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
				OnSizeChange(size, newsize);
			}
		}
		TinyRectangle TinyVisual::GetPadding() const
		{
			return m_padding;
		}
		TinyRectangle TinyVisual::GetMargin() const
		{
			return m_margin;
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
			return static_cast<HRESULT>(::SendMessage(m_document->Handle(), uMsg, wParam, lParam));
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
			EVENT_CREATE(this, EventArgs());
			return TRUE;
		}
		HRESULT	TinyVisual::OnDestory()
		{
			EVENT_DESTORY(this, EventArgs());
			return TRUE;
		}
		HRESULT	TinyVisual::OnInitialize()
		{
			EVENT_INITIALIZE(this, EventArgs());
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
			EVENT_MOUSEMOVE(this, MouseEventArgs(button, 0, pos.x, pos.y, 0));
			return FALSE;
		}
		HRESULT TinyVisual::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			MouseButtons button = NONE;
			if (dwFlags & MK_LBUTTON) button = MouseButtons::LBUTTON;
			if (dwFlags & MK_RBUTTON) button = MouseButtons::RBUTTON;
			if (dwFlags & MK_MBUTTON) button = MouseButtons::MBUTTON;
			EVENT_MOUSEMOVE(this, MouseEventArgs(button, 0, pos.x, pos.y, zDelta));
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
			EVENT_MOUSEDOWN(this, MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(this, MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MOUSECLICK(this, MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(this, MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDOWN(this, MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(this, MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MOUSECLICK(this, MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(this, MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDOWN(this, MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEUP(this, MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MOUSEDBCLICK(this, MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KEYDOWN(this, KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KEYUP(this, KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_CHAR(this, KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnFocus(BOOL bFlag)
		{
			EVENT_FOCUS(this, FocusEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnCapture(BOOL bFlag)
		{
			EVENT_CAPTURE(this, CaptureEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnActive(BOOL bFlag)
		{
			EVENT_ACTIVE(this, ActiveEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	 TinyVisual::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(hWND, &pos);
			if (m_szCursor != TinyVisualCursor::AUTO)
			{
				if (m_document != NULL)
				{
					TinyRectangle s = m_document->GetWindowRect(this);
					if (s.PtInRect(pos))
					{
						m_document->GetVisualHWND().SetMsgHandled(TRUE);
						::SetCursor(LoadCursor(NULL, CursorFromString(m_szCursor)));
						return TRUE;
					}
				}

			}
			return FALSE;
		}
		TinyString TinyVisual::GetProperty(const TinyString& name)
		{
			ITERATOR pos = m_propertys.First();
			while (pos != NULL)
			{
				TinyVisualProperty& value = m_propertys.GetAt(pos);
				if (value.name() == name)
				{
					return value.value();
				}
				pos = m_propertys.Next(pos);
			}
			return TinyString();
		}
		BOOL TinyVisual::IsPropertyNull(const TinyString& name)
		{
			ITERATOR pos = m_propertys.First();
			while (pos != NULL)
			{
				TinyVisualProperty& value = m_propertys.GetAt(pos);
				if (value.name() == name)
				{
					return TRUE;
				}
				pos = m_propertys.Next(pos);
			}
			return FALSE;
		}
		BOOL TinyVisual::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::NAME.STR()) == 0)
			{
				this->SetName(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::TEXT.STR()) == 0)
			{
				this->SetText(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::CURSOR.STR()) == 0)
			{
				this->SetCursor(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BACKGROUNDIMAGE.STR()) == 0)
			{
				this->SetBackgroundImage(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BACKGROUNDCOLOR.STR()) == 0)
			{
				this->SetBackgroundColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BACKGROUNDCENTER.STR()) == 0)
			{
				this->SetBackgroundCenter(TinyVisualBuilder::GetRectangle(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BACKGROUNDSIZE.STR()) == 0)
			{
				this->SetBackgroundSize(TinyVisualBuilder::GetSize(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BACKGROUNDPOSITION.STR()) == 0)
			{
				this->SetBackgroundPosition(TinyVisualBuilder::GetPosition(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BORDERCOLOR.STR()) == 0)
			{
				this->SetBorderColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BORDERWIDTH.STR()) == 0)
			{
				this->SetBorderThickness(TinyVisualBuilder::GetInt32(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BORDERSTYLE.STR()) == 0)
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
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BORDERIMAGE.STR()) == 0)
			{
				this->SetBorderImage(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::BORDERCENTER.STR()) == 0)
			{
				this->SetBorderCenter(TinyVisualBuilder::GetRectangle(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::TOOLTIP.STR()) == 0)
			{
				this->SetToolTip(value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::ENABLE.STR()) == 0)
			{
				this->SetEnable(TinyVisualBuilder::GetBool(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::VISIBLE.STR()) == 0)
			{
				this->SetVisible(TinyVisualBuilder::GetBool(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::POSITION.STR()) == 0)
			{
				this->SetPosition(TinyVisualBuilder::GetPosition(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::SIZE.STR()) == 0)
			{
				this->SetSize(TinyVisualBuilder::GetSize(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::PADDING.STR()) == 0)
			{
				this->SetPadding(TinyVisualBuilder::GetRectangle(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::MARGIN.STR()) == 0)
			{
				this->SetMargin(TinyVisualBuilder::GetRectangle(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::MAXSIZE.STR()) == 0)
			{
				TinySize maxsize = TinyVisualBuilder::GetSize(value);
				this->SetMaximumSize(maxsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::TEXTCOLOR.STR()) == 0)
			{
				this->SetTextColor(TinyVisualBuilder::GetColor(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::TEXTALIGN.STR()) == 0)
			{
				this->SetTextAlian(TinyVisualBuilder::GetAlign(value));
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::FONTFAMILY.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				strcpy_s(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::FONTWEIGHT.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfCharSet = GB2312_CHARSET;
				lf.lfWeight = atoi(value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::FONTSIZE.STR()) == 0)
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
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::FONTSTYLE.STR()) == 0)
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
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::TEXTDECORATION.STR()) == 0)
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
