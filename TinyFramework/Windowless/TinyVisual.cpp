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
			m_count(0),
			m_backgroundImage(NULL),
			m_backgroundColor(DEFAULT_COLOR),
			m_textColor(DEFAULT_COLOR),
			m_textAlign(DT_LEFT)
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
			m_count(0),
			m_backgroundImage(NULL),
			m_backgroundColor(DEFAULT_COLOR),
			m_textColor(DEFAULT_COLOR),
			m_textAlign(DT_LEFT)
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
				TinyVisual* pv = NULL;
				for (pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv)
						continue;
					pv->SetVisible(visible);
				}
			}
		}
		void TinyVisual::SetEnable(BOOL enable)
		{
			if (m_enable != enable)
			{
				m_enable = enable;
				TinyVisual* pv = NULL;
				for (pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv)
						continue;
					pv->SetEnable(enable);
				}
			}
		}

		void TinyVisual::SetPadding(const TinyRectangle& s)
		{
			if (m_padding != s)
			{
				m_padding = s;
			}
		}

		void TinyVisual::SetBackgroundImage(TinyImage* image)
		{
			if (m_backgroundImage != image)
			{
				m_backgroundImage = image;
			}
		}
		void TinyVisual::SetBackgroundColor(COLORREF color)
		{
			if (m_backgroundColor != color)
			{
				m_backgroundColor = color;
			}
		}
		void TinyVisual::SetBackgroundRectangle(const TinyRectangle& rectangle)
		{
			if (m_backgroundRectangle != rectangle)
			{
				m_backgroundRectangle = rectangle;
			}
		}

		void TinyVisual::SetBackgroundCenter(const TinyRectangle& center)
		{
			if (m_backgroundCenter != center)
			{
				m_backgroundCenter = center;
			}
		}

		void TinyVisual::SetBorderThickness(const TinyRectangle& thickness)
		{
			if (m_borderThickness != thickness)
			{
				m_borderThickness = thickness;
			}
		}
		void TinyVisual::SetBorderColor(COLORREF color)
		{
			if (m_borderColor != color)
			{
				m_borderColor = color;
			}
		}
		void TinyVisual::SetBorderStyle(UINT style)
		{
			if (m_borderStyle != style)
			{
				m_borderStyle = style;
			}
		}

		void TinyVisual::SetTextColor(COLORREF color)
		{
			if (m_textColor != color)
			{
				m_textColor = color;
			}
		}
		void TinyVisual::SetTextAlian(UINT textAlign)
		{
			if (m_textAlign != textAlign)
			{
				m_textAlign = textAlign;
			}
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
		TinyRectangle TinyVisual::GetPadding() const
		{
			return m_padding;
		}
		TinyVisualDocument*	TinyVisual::GetDocument()
		{
			return m_document;
		}
		UINT TinyVisual::GetChildCount() const
		{
			return m_count;
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

		TinyRectangle TinyVisual::GetBackgroundRectangle() const
		{
			return m_backgroundRectangle;
		}
		COLORREF TinyVisual::GetBackgroundColor() const
		{
			return  m_backgroundColor;
		}
		TinyImage* TinyVisual::GetBackgroundImage() const
		{
			return m_backgroundImage;
		}
		TinyRectangle TinyVisual::GetBackgroundCenter() const
		{
			return m_backgroundCenter;
		}
		TinyRectangle TinyVisual::GetBorderThickness() const
		{
			return m_borderThickness;
		}
		COLORREF TinyVisual::GetBorderColor() const
		{
			return m_borderColor;
		}
		UINT TinyVisual::GetBorderStyle() const
		{
			return m_borderStyle;
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
			ASSERT(m_document);
			return TRUE;
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
		HRESULT	 TinyVisual::OnMouseHover(const TinyPoint& pos, DWORD dwFlags)
		{
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
	}
}
