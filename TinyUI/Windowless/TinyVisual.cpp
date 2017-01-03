#include "../stdafx.h"
#include "TinyVisual.h"
#include "TinyVisualDocument.h"

namespace TinyUI
{
	namespace Windowless
	{
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
			m_dwCount(0)
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
		BOOL TinyVisual::IsLayout() const
		{
			return FALSE;
		}
		HWND TinyVisual::Handle() const
		{
			return m_document != NULL ? m_document->Handle() : NULL;
		}
		void TinyVisual::SetText(LPCSTR pzText)
		{
			m_strText = pzText;
		}
		TinyString	TinyVisual::GetText() const
		{
			return m_strText;
		}
		void TinyVisual::SetName(LPCSTR pzName)
		{
			m_strName = pzName;
		}
		TinyString	TinyVisual::GetName() const
		{
			return m_strName;
		}
		void TinyVisual::SetToolTip(LPCSTR pzTitle)
		{
			m_strToolTip = pzTitle;
		}
		TinyString	TinyVisual::GetToolTip() const
		{
			return m_strToolTip;
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
			return FALSE;
		}
		HRESULT	TinyVisual::OnDestory()
		{
			return FALSE;
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
			EVENT_MouseMove(MouseEventArgs(button, 0, pos.x, pos.y, 0));
			return FALSE;
		}
		HRESULT TinyVisual::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			MouseButtons button = NONE;
			if (dwFlags & MK_LBUTTON) button = MouseButtons::LBUTTON;
			if (dwFlags & MK_RBUTTON) button = MouseButtons::RBUTTON;
			if (dwFlags & MK_MBUTTON) button = MouseButtons::MBUTTON;
			EVENT_MouseMove(MouseEventArgs(button, 0, pos.x, pos.y, zDelta));
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
			EVENT_MouseDown(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MouseClick(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseDown(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MouseClick(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseDown(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KeyDown(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KeyUp(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_Char(KeyEventArgs(dwChar, dwRepCnt, dwFlags));
			return FALSE;
		}
		HRESULT	TinyVisual::OnFocus(BOOL bFlag)
		{
			EVENT_Focus(FocusEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnCapture(BOOL bFlag)
		{
			EVENT_Capture(CaptureEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	TinyVisual::OnActive(BOOL bFlag)
		{
			EVENT_Active(ActiveEventArgs(bFlag));
			return FALSE;
		}
		HRESULT	 TinyVisual::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			return FALSE;
		}
		HRESULT	TinyVisual::SetProperty(const TinyString& name, const TinyString& value)
		{
			HRESULT hRes = S_OK;
			if (strcasecmp(name.STR(), TinyVisualProperty::NAME.STR()) == 0)
			{
				this->SetName(value.STR());
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXT.STR()) == 0)
			{
				this->SetText(value.STR());
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TOOLTIP.STR()) == 0)
			{
				this->SetToolTip(value.STR());
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ENABLE.STR()) == 0)
			{
				this->SetEnable(TinyVisualBuilder::GetBool(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::VISIBLE.STR()) == 0)
			{
				this->SetVisible(TinyVisualBuilder::GetBool(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::POSITION.STR()) == 0)
			{
				this->SetPosition(TinyVisualBuilder::GetPosition(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SIZE.STR()) == 0)
			{
				this->SetSize(TinyVisualBuilder::GetSize(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MAXSIZE.STR()) == 0)
			{
				TinySize maxsize = TinyVisualBuilder::GetSize(value);
				this->SetMaximumSize(maxsize);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::MINSIZE.STR()) == 0)
			{
				TinySize minsize = TinyVisualBuilder::GetSize(value);
				this->SetMinimumSize(minsize);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXTCOLOR.STR()) == 0)
			{
				this->SetTextColor(TinyVisualBuilder::GetColor(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::TEXTALIGN.STR()) == 0)
			{
				this->SetTextAlian(TinyVisualBuilder::GetAlign(value));
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTFAMILY.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				strcpy(lf.lfFaceName, value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTWEIGHT.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfWeight = atoi(value.STR());
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTSIZE.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				TinySize size = TinyVisualBuilder::GetSize(value);
				lf.lfWidth = size.cx;
				lf.lfHeight = size.cy;
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::FONTSTYLE.STR()) == 0)
			{
				LOGFONT lf;
				::GetObject(m_hFONT, sizeof(LOGFONT), &lf);
				lf.lfItalic = FALSE;
				if (strcasecmp(value.STR(), "italic") == 0)
					lf.lfItalic = TRUE;
				if (strcasecmp(value.STR(), "oblique") == 0)
					lf.lfItalic = TRUE;
				if (strcasecmp(value.STR(), "normal") == 0)
					lf.lfItalic = FALSE;
				SAFE_DELETE_OBJECT(m_hFONT);
				m_hFONT = CreateFontIndirect(&lf);
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
			}
			return S_OK;
		}
	}
}
