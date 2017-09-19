#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualScrollBar.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualHScrollBar, TinyVisual);

		TinyVisualHScrollBar::TinyVisualHScrollBar()
		{

		}
		TinyVisualHScrollBar::TinyVisualHScrollBar(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			memset(&m_si, 0, sizeof(SCROLLBOXINFO));
			m_si.iHitTest = m_si.iHitTestPress = HTSCROLL_NONE;
			ZeroMemory(&m_images, sizeof(m_images));
		}
		TinyVisualHScrollBar::~TinyVisualHScrollBar()
		{

		}
		TinyString TinyVisualHScrollBar::RetrieveTag() const
		{
			return TinyVisualTag::HSCROLLBAR;
		}
		BOOL TinyVisualHScrollBar::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1NORMAL.STR()) == 0)
			{
				this->SetStyleImage(ARROW1NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1HIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(ARROW1HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1DOWN.STR()) == 0)
			{
				this->SetStyleImage(ARROW1DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2NORMAL.STR()) == 0)
			{
				this->SetStyleImage(ARROW2NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2HIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(ARROW2HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2DOWN.STR()) == 0)
			{
				this->SetStyleImage(ARROW2DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARGROOVE.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARGROOVE, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARNORMAL.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARNORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARHIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARHIGHLIGHT, value.STR());
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}
		void TinyVisualHScrollBar::SetSize(const TinySize& size)
		{
			TinyVisual::SetSize(size);
			SetScrollInfo(m_si.iMin, m_si.iMax, size.cx, m_si.iPos);
		}
		HRESULT TinyVisualHScrollBar::OnCreate()
		{
			m_images[ARROW1NORMAL] = TinyVisualResource::GetInstance()["hscrollbar_arrow_up_normal"];
			m_images[ARROW1HIGHLIGHT] = TinyVisualResource::GetInstance()["hscrollbar_arrow_up_hover"];
			m_images[ARROW1DOWN] = TinyVisualResource::GetInstance()["hscrollbar_arrow_up_press"];
			m_images[ARROW2NORMAL] = TinyVisualResource::GetInstance()["hscrollbar_arrow_down_normal"];
			m_images[ARROW2HIGHLIGHT] = TinyVisualResource::GetInstance()["hscrollbar_arrow_down_hover"];
			m_images[ARROW2DOWN] = TinyVisualResource::GetInstance()["hscrollbar_arrow_down_normal"];
			m_images[SCROLLBARGROOVE] = TinyVisualResource::GetInstance()["hscrollbar_groove"];
			m_images[SCROLLBARNORMAL] = TinyVisualResource::GetInstance()["hscrollbar_normal"];
			m_images[SCROLLBARHIGHLIGHT] = TinyVisualResource::GetInstance()["hscrollbar_hover"];
			return S_OK;
		}
		BOOL TinyVisualHScrollBar::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			TinyRectangle clip = m_document->GetWindowRect(this);
			DrawScrollBar(canvas, clip.Size());
			return FALSE;
		}
		HRESULT	TinyVisualHScrollBar::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			if (dwFlags & MK_LBUTTON)
			{
				if (m_si.iHitTest == HTSCROLL_THUMB)
				{
					SCROLLBARCALC si = { 0 };
					TinySize size = m_rectangle.Size();
					ScrollCalculate(&si, size);
					ScrollTrackThumb(pos, &si);
					TinyRectangle s = m_document->GetWindowRect(this);
					m_document->Invalidate(&s);
					return TinyVisual::OnMouseMove(pos, dwFlags);
				}
			}
			INT iHitTest = ScrollHitTest(pos);
			if (m_si.iHitTest != iHitTest)
			{
				m_si.iHitTest = iHitTest;
				TinyRectangle s = m_document->GetWindowRect(this);
				m_document->Invalidate(&s);
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	TinyVisualHScrollBar::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			zDelta = zDelta / 10;
			INT iNewPos = m_si.iPos;
			if (zDelta > 0)
			{
				iNewPos = (m_si.iPos - abs(zDelta)) > m_si.iMin ? m_si.iPos - abs(zDelta) : m_si.iMin;
			}
			else
			{
				iNewPos = (m_si.iPos + abs(zDelta)) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + abs(zDelta) : (m_si.iMax - m_si.iPage + 1);
			}
			if (iNewPos != m_si.iPos)
			{
				INT iOldPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(FALSE, zDelta > 0 ? SB_LINEDOWN : SB_LINEUP, iOldPos, iNewPos);
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			return FALSE;
		}
		HRESULT	TinyVisualHScrollBar::OnMouseLeave()
		{
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualHScrollBar::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			INT iHitTest = ScrollHitTest(pos);
			switch (iHitTest)
			{
			case HTSCROLL_LINEDOWN:
				m_si.iHitTestPress = HTSCROLL_LINEDOWN;
				break;
			case HTSCROLL_LINEUP:
				m_si.iHitTestPress = HTSCROLL_LINEUP;
				break;
			case HTSCROLL_THUMB:
				SCROLLBARCALC si = { 0 };
				TinySize size = m_rectangle.Size();
				ScrollCalculate(&si, size);
				m_si.iThumbOffset = pos.x - si.thumbRectangle.left;
				break;
			}
			m_si.iHitTest = iHitTest;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			m_document->SetCapture(this);
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualHScrollBar::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			INT iNewPos = m_si.iPos;
			INT code = 0;
			switch (m_si.iHitTest)
			{
			case HTSCROLL_LINEUP:
				code = SB_LINEUP;
				iNewPos = (m_si.iPos - 1) > m_si.iMin ? m_si.iPos - 1 : m_si.iMin;
				break;
			case HTSCROLL_LINEDOWN:
				code = SB_LINEDOWN;
				iNewPos = (m_si.iPos + 1) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + 1 : (m_si.iMax - m_si.iPage + 1);
				break;
			case HTSCROLL_PAGEUP:
				code = SB_PAGEUP;
				iNewPos = (m_si.iPos - m_si.iPage) > m_si.iMin ? m_si.iPos - m_si.iPage : m_si.iMin;
				break;
			case HTSCROLL_PAGEDOWN:
				code = SB_PAGEDOWN;
				iNewPos = (m_si.iPos + m_si.iPage) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + m_si.iPage : (m_si.iMax - m_si.iPage + 1);
				break;
			case HTSCROLL_THUMB:
				code = SB_THUMBPOSITION;
				EVENT_PosChange(FALSE, code, m_si.iPos, iNewPos);
				break;
			}
			if (iNewPos != m_si.iPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(FALSE, code, iPos, iNewPos);
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			m_si.iThumbOffset = 0;
			m_document->ReleaseCapture();
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		INT	TinyVisualHScrollBar::ScrollHitTest(const TinyPoint& pt)
		{
			SCROLLBARCALC si = { 0 };
			TinySize size = m_rectangle.Size();
			ScrollCalculate(&si, size);
			if (pt.x >= si.arrowRectangle[0].left && pt.x < si.arrowRectangle[0].right)
				return HTSCROLL_LINEUP;
			if (pt.x >= si.pageRectangle[0].left && pt.x < si.pageRectangle[0].right)
				return HTSCROLL_PAGEUP;
			if (pt.x >= si.thumbRectangle.left && pt.x < si.thumbRectangle.right)
				return HTSCROLL_THUMB;
			if (pt.x >= si.pageRectangle[1].left && pt.x < si.pageRectangle[1].right)
				return HTSCROLL_PAGEDOWN;
			if (pt.x >= si.arrowRectangle[1].left && pt.x < si.arrowRectangle[1].right)
				return HTSCROLL_LINEDOWN;
			return HTSCROLL_NONE;
		}
		void TinyVisualHScrollBar::ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size)
		{
			ASSERT(ps);
			::SetRect(&ps->rectangle,
				TINY_SCROLL_MARGIN_LEFT,
				TINY_SCROLL_MARGIN_TOP,
				size.cx - TINY_SCROLL_MARGIN_RIGHT,
				size.cy - TINY_SCROLL_MARGIN_BOTTOM);
			::SetRect(&ps->arrowRectangle[0],
				ps->rectangle.left,
				ps->rectangle.top,
				ps->rectangle.left + TINY_SM_CXSCROLL,
				ps->rectangle.bottom);//上箭头的高度
			::SetRect(&ps->arrowRectangle[1],
				ps->rectangle.right - TINY_SM_CXSCROLL,
				ps->rectangle.top,
				ps->rectangle.right,
				ps->rectangle.bottom);//下箭头的大小
			INT iRange = (m_si.iMax - m_si.iMin) + 1;
			INT iGrooveSize = ps->arrowRectangle[1].left - ps->arrowRectangle[0].right;//划块的槽
			BOOL bFlag = (m_si.iPage > m_si.iMax || m_si.iMax <= m_si.iMin || m_si.iMax == 0);
			if (iRange > 0 && !bFlag)
			{
				//计算划块的大小
				INT iThumbPos = 0;
				INT iThumbSize = MulDiv(m_si.iPage, iGrooveSize, iRange);
				if (iThumbSize < TINY_SCROLL_MINTHUMB_SIZE)
					iThumbSize = TINY_SCROLL_MINTHUMB_SIZE;
				//计算Page
				INT iPageSize = max(1, m_si.iPage);
				iThumbPos = MulDiv(m_si.iPos - m_si.iMin, iGrooveSize - iThumbSize, iRange - iPageSize);
				if (iThumbPos < 0)
					iThumbPos = 0;
				if (iThumbPos >= (iGrooveSize - iThumbSize))
					iThumbPos = iGrooveSize - iThumbSize;
				iThumbPos += ps->rectangle.left + TO_CX(ps->arrowRectangle[0]);
				::SetRect(&ps->thumbRectangle,
					iThumbPos,
					ps->rectangle.top,
					iThumbPos + iThumbSize,
					ps->rectangle.bottom);
				::SetRect(&ps->pageRectangle[0],
					ps->arrowRectangle[0].right,
					ps->rectangle.top,
					ps->thumbRectangle.left,
					ps->rectangle.bottom);
				::SetRect(&ps->pageRectangle[1],
					ps->thumbRectangle.right,
					ps->rectangle.top,
					ps->arrowRectangle[1].left,
					ps->rectangle.bottom);
			}
			else
			{
				::SetRect(&ps->thumbRectangle,
					0,
					ps->rectangle.top,
					0,
					ps->rectangle.bottom);
				::SetRect(&ps->pageRectangle[0],
					ps->arrowRectangle[0].right,
					ps->rectangle.top,
					ps->thumbRectangle.left,
					ps->rectangle.bottom);
				::SetRect(&ps->pageRectangle[1],
					ps->thumbRectangle.right,
					ps->rectangle.top,
					ps->arrowRectangle[1].left,
					ps->rectangle.bottom);
			}
		}
		void TinyVisualHScrollBar::ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps)
		{
			ASSERT(ps);
			INT iNewPos = 0;
			INT iRange = (m_si.iMax - m_si.iMin) + 1;
			INT iThumbPos = (pt.x - m_si.iThumbOffset) - ps->arrowRectangle[0].right;
			INT iThumbSize = TO_CX(ps->thumbRectangle);
			INT iGrooveSize = (ps->arrowRectangle[1].left - ps->arrowRectangle[0].right);
			if (iThumbPos < 0)
				iThumbPos = 0;
			if (iThumbPos > (iGrooveSize - iThumbSize))
				iThumbPos = iGrooveSize - iThumbSize;
			if (iRange > 0)
			{
				iNewPos = m_si.iMin + MulDiv(iThumbPos, iRange - m_si.iPage, iGrooveSize - iThumbSize);
			}
			if (m_si.iPos != iNewPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(FALSE, SB_THUMBTRACK, iPos, iNewPos);
			}
		}
		void TinyVisualHScrollBar::DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size)
		{
			SCROLLBARCALC si = { 0 };
			ScrollCalculate(&si, size);
			DrawArrow(canvas, &si);
			DrawGroove(canvas, &si);
			DrawThumb(canvas, &si);
		}
		void TinyVisualHScrollBar::DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			TinyRectangle s = this->GetWindowRect();
			TinyRectangle arrowRectangle0 = ps->arrowRectangle[0];
			OffsetRect(&arrowRectangle0, s.left, s.top);
			TinyRectangle arrowRectangle1 = ps->arrowRectangle[1];
			OffsetRect(&arrowRectangle1, s.left, s.top);
			if (m_si.iHitTest == HTSCROLL_LINEUP)
			{
				if (m_si.iHitTestPress == HTSCROLL_LINEUP)
				{
					canvas.DrawImage(*m_images[ARROW1DOWN], arrowRectangle0, m_images[ARROW1DOWN]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
				}
				else
				{
					canvas.DrawImage(*m_images[ARROW1HIGHLIGHT], arrowRectangle0, m_images[ARROW1HIGHLIGHT]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
				}
			}
			else if (m_si.iHitTest == HTSCROLL_LINEDOWN)
			{
				if (m_si.iHitTestPress == HTSCROLL_LINEDOWN)
				{
					canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2DOWN], arrowRectangle1, m_images[ARROW2DOWN]->GetRectangle());
				}
				else
				{
					canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2HIGHLIGHT], arrowRectangle1, m_images[ARROW2HIGHLIGHT]->GetRectangle());
				}
			}
			else
			{
				canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
				canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
			}
		}
		void TinyVisualHScrollBar::DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			if (m_si.iHitTest == HTSCROLL_THUMB)
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, m_images[SCROLLBARHIGHLIGHT]->GetRectangle());
				srcCenter.left = srcCenter.left + 4;
				srcCenter.right = srcCenter.right - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.left = dstCenter.left + 4;
				dstCenter.right = dstCenter.right - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(*m_images[SCROLLBARHIGHLIGHT], thumbRectangle, dstCenter, m_images[SCROLLBARHIGHLIGHT]->GetRectangle(), srcCenter);
			}
			else
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, m_images[SCROLLBARNORMAL]->GetRectangle());
				srcCenter.left = srcCenter.left + 4;
				srcCenter.right = srcCenter.right - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.left = dstCenter.left + 4;
				dstCenter.right = dstCenter.right - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(*m_images[SCROLLBARNORMAL], thumbRectangle, dstCenter, m_images[SCROLLBARNORMAL]->GetRectangle(), srcCenter);
			}
		}
		void TinyVisualHScrollBar::DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			RECT dstCenter, srcCenter;
			CopyRect(&srcCenter, m_images[SCROLLBARNORMAL]->GetRectangle());
			srcCenter.left = srcCenter.left + 4;
			srcCenter.right = srcCenter.right - 4;
			RECT grooveRectangle;
			CopyRect(&grooveRectangle, &ps->rectangle);
			grooveRectangle.left = ps->arrowRectangle[0].right;
			grooveRectangle.right = ps->arrowRectangle[1].left;
			CopyRect(&dstCenter, &grooveRectangle);
			dstCenter.left = dstCenter.left + 4;
			dstCenter.right = dstCenter.right - 4;
			TinyRectangle s = this->GetWindowRect();
			OffsetRect(&grooveRectangle, s.left, s.top);
			OffsetRect(&dstCenter, s.left, s.top);
			canvas.DrawImage(*m_images[SCROLLBARNORMAL], grooveRectangle, dstCenter, m_images[SCROLLBARNORMAL]->GetRectangle(), srcCenter);
		}
		void TinyVisualHScrollBar::SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos)
		{
			m_si.iPos = iPos < iMin ? iMin : iPos;
			m_si.iPos = iPos > (iMax - iPage + 1) ? (iMax - iPage + 1) : iPos;
			if (iMin <= m_si.iPos && m_si.iPos <= (iMax - iPage + 1))
			{
				m_si.iMin = iMin;
				m_si.iMax = iMax;
				m_si.iPage = iPage;
			}
			else
			{
				m_si.iPos = m_si.iMin = m_si.iMax = m_si.iMin = 0;
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
		void TinyVisualHScrollBar::SetScrollPos(INT iPos)
		{
			iPos = iPos < m_si.iMin ? m_si.iMin : iPos;
			iPos = iPos > (m_si.iMax - m_si.iPage + 1) ? (m_si.iMax - m_si.iPage + 1) : iPos;
			m_si.iPos = iPos;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
		INT	TinyVisualHScrollBar::GetScrollPos() const
		{
			return m_si.iPos;
		}
		INT TinyVisualHScrollBar::GetPage() const
		{
			return m_si.iPage;
		}
		INT	TinyVisualHScrollBar::GetMin() const
		{
			return m_si.iMin;
		}
		INT	TinyVisualHScrollBar::GetMax() const
		{
			return m_si.iMax;
		}
		void TinyVisualHScrollBar::SetStyleImage(ScrollImageStyle type, LPCSTR pzFile)
		{
			m_images[type] = TinyVisualResource::GetInstance()[pzFile];
			ASSERT(m_images[type]);
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNCREATE(TinyVisualVScrollBar, TinyVisual);

		TinyVisualVScrollBar::TinyVisualVScrollBar()
		{

		}
		TinyVisualVScrollBar::TinyVisualVScrollBar(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			memset(&m_si, 0, sizeof(SCROLLBOXINFO));
			m_si.iHitTest = m_si.iHitTestPress = HTSCROLL_NONE;
			ZeroMemory(&m_images, sizeof(m_images));
		}

		HRESULT TinyVisualVScrollBar::OnCreate()
		{
			m_images[ARROW1NORMAL] = TinyVisualResource::GetInstance()["vscrollbar_arrow_up_normal"];
			m_images[ARROW1HIGHLIGHT] = TinyVisualResource::GetInstance()["vscrollbar_arrow_up_hover"];
			m_images[ARROW1DOWN] = TinyVisualResource::GetInstance()["vscrollbar_arrow_up_press"];
			m_images[ARROW2NORMAL] = TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"];
			m_images[ARROW2HIGHLIGHT] = TinyVisualResource::GetInstance()["vscrollbar_arrow_down_hover"];
			m_images[ARROW2DOWN] = TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"];
			m_images[SCROLLBARGROOVE] = TinyVisualResource::GetInstance()["vscrollbar_groove"];
			m_images[SCROLLBARNORMAL] = TinyVisualResource::GetInstance()["vscrollbar_normal"];
			m_images[SCROLLBARHIGHLIGHT] = TinyVisualResource::GetInstance()["vscrollbar_hover"];
			return S_OK;
		}

		TinyVisualVScrollBar::~TinyVisualVScrollBar()
		{

		}
		TinyString TinyVisualVScrollBar::RetrieveTag() const
		{
			return TinyVisualTag::VSCROLLBAR;
		}
		BOOL TinyVisualVScrollBar::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1NORMAL.STR()) == 0)
			{
				this->SetStyleImage(ARROW1NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1HIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(ARROW1HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW1DOWN.STR()) == 0)
			{
				this->SetStyleImage(ARROW1DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2NORMAL.STR()) == 0)
			{
				this->SetStyleImage(ARROW2NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2HIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(ARROW2HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROW2DOWN.STR()) == 0)
			{
				this->SetStyleImage(ARROW2DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARGROOVE.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARGROOVE, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARNORMAL.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARNORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SCROLLBARHIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(SCROLLBARHIGHLIGHT, value.STR());
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}
		BOOL TinyVisualVScrollBar::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			TinyRectangle clip = m_document->GetWindowRect(this);
			DrawScrollBar(canvas, clip.Size());
			return FALSE;
		}
		HRESULT	TinyVisualVScrollBar::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			if (dwFlags & MK_LBUTTON)
			{
				if (m_si.iHitTest == HTSCROLL_THUMB)
				{
					SCROLLBARCALC si = { 0 };
					TinySize size = m_rectangle.Size();
					ScrollCalculate(&si, size);
					ScrollTrackThumb(pos, &si);
					TinyRectangle s = m_document->GetWindowRect(this);
					m_document->Invalidate(&s);
					return TinyVisual::OnMouseMove(pos, dwFlags);
				}
			}
			INT iHitTest = ScrollHitTest(pos);
			if (m_si.iHitTest != iHitTest)
			{
				m_si.iHitTest = iHitTest;
				TinyRectangle s = m_document->GetWindowRect(this);
				m_document->Invalidate(&s);
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	TinyVisualVScrollBar::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			zDelta = zDelta / 10;
			INT iNewPos = m_si.iPos;
			if (zDelta > 0)
			{
				iNewPos = (m_si.iPos - abs(zDelta)) > m_si.iMin ? m_si.iPos - abs(zDelta) : m_si.iMin;
			}
			else
			{
				iNewPos = (m_si.iPos + abs(zDelta)) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + abs(zDelta) : (m_si.iMax - m_si.iPage + 1);
			}
			if (iNewPos != m_si.iPos)
			{
				INT iOldPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(TRUE, zDelta > 0 ? SB_LINEDOWN : SB_LINEUP, iOldPos, iNewPos);
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			return FALSE;
		}
		HRESULT	TinyVisualVScrollBar::OnMouseLeave()
		{
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualVScrollBar::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			INT iHitTest = ScrollHitTest(pos);
			switch (iHitTest)
			{
			case HTSCROLL_LINEDOWN:
				m_si.iHitTestPress = HTSCROLL_LINEDOWN;
				break;
			case HTSCROLL_LINEUP:
				m_si.iHitTestPress = HTSCROLL_LINEUP;
				break;
			case HTSCROLL_THUMB:
				SCROLLBARCALC si = { 0 };
				TinySize size = m_rectangle.Size();
				ScrollCalculate(&si, size);
				m_si.iThumbOffset = pos.y - si.thumbRectangle.top;
				break;
			}
			m_si.iHitTest = iHitTest;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			m_document->SetCapture(this);
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualVScrollBar::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			INT iNewPos = m_si.iPos;
			INT code = 0;
			switch (m_si.iHitTest)
			{
			case HTSCROLL_LINEUP:
				code = SB_LINEUP;
				iNewPos = (m_si.iPos - 1) > m_si.iMin ? m_si.iPos - 1 : m_si.iMin;
				break;
			case HTSCROLL_LINEDOWN:
				code = SB_LINEDOWN;
				iNewPos = (m_si.iPos + 1) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + 1 : (m_si.iMax - m_si.iPage + 1);
				break;
			case HTSCROLL_PAGEUP:
				code = SB_PAGEUP;
				iNewPos = (m_si.iPos - m_si.iPage) > m_si.iMin ? m_si.iPos - m_si.iPage : m_si.iMin;
				break;
			case HTSCROLL_PAGEDOWN:
				code = SB_PAGEDOWN;
				iNewPos = (m_si.iPos + m_si.iPage) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + m_si.iPage : (m_si.iMax - m_si.iPage + 1);
				break;
			case HTSCROLL_THUMB:
				code = SB_THUMBPOSITION;
				EVENT_PosChange(TRUE, code, m_si.iPos, iNewPos);
				break;
			}
			if (iNewPos != m_si.iPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(TRUE, code, iPos, iNewPos);
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			m_si.iThumbOffset = 0;
			m_document->ReleaseCapture();
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		INT	TinyVisualVScrollBar::ScrollHitTest(const TinyPoint& pt)
		{
			SCROLLBARCALC si = { 0 };
			TinySize size = m_rectangle.Size();
			ScrollCalculate(&si, size);
			if (pt.y >= si.arrowRectangle[0].top && pt.y < si.arrowRectangle[0].bottom)
				return HTSCROLL_LINEUP;
			if (pt.y >= si.pageRectangle[0].top && pt.y < si.pageRectangle[0].bottom)
				return HTSCROLL_PAGEUP;
			if (pt.y >= si.thumbRectangle.top && pt.y < si.thumbRectangle.bottom)
				return HTSCROLL_THUMB;
			if (pt.y >= si.pageRectangle[1].top && pt.y < si.pageRectangle[1].bottom)
				return HTSCROLL_PAGEDOWN;
			if (pt.y >= si.arrowRectangle[1].top && pt.y < si.arrowRectangle[1].bottom)
				return HTSCROLL_LINEDOWN;
			return HTSCROLL_NONE;
		}
		void TinyVisualVScrollBar::ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size)
		{
			::SetRect(&ps->rectangle,
				TINY_SCROLL_MARGIN_LEFT,
				TINY_SCROLL_MARGIN_TOP,
				size.cx - TINY_SCROLL_MARGIN_RIGHT,
				size.cy - TINY_SCROLL_MARGIN_BOTTOM);
			::SetRect(&ps->arrowRectangle[0],
				ps->rectangle.left,
				ps->rectangle.top,
				ps->rectangle.right,
				ps->rectangle.top + TINY_SM_CYSCROLL);//上箭头的高度
			::SetRect(&ps->arrowRectangle[1],
				ps->rectangle.left,
				ps->rectangle.bottom - TINY_SM_CYSCROLL,
				ps->rectangle.right,
				ps->rectangle.bottom);//下箭头的大小
			INT iRange = (m_si.iMax - m_si.iMin) + 1;
			INT iGrooveSize = ps->arrowRectangle[1].top - ps->arrowRectangle[0].bottom;//划块的槽
			BOOL bFlag = (m_si.iPage > m_si.iMax || m_si.iMax <= m_si.iMin || m_si.iMax == 0);
			if (iRange > 0 && !bFlag)
			{
				//计算划块的大小
				INT iThumbPos = 0;
				INT iThumbSize = MulDiv(m_si.iPage, iGrooveSize, iRange);
				if (iThumbSize < TINY_SCROLL_MINTHUMB_SIZE)
					iThumbSize = TINY_SCROLL_MINTHUMB_SIZE;
				//计算Page
				INT iPageSize = max(1, m_si.iPage);
				iThumbPos = MulDiv(m_si.iPos - m_si.iMin, iGrooveSize - iThumbSize, iRange - iPageSize);
				if (iThumbPos < 0)
					iThumbPos = 0;
				if (iThumbPos >= (iGrooveSize - iThumbSize))
					iThumbPos = iGrooveSize - iThumbSize;
				iThumbPos += ps->rectangle.top + TO_CY(ps->arrowRectangle[0]);
				::SetRect(&ps->thumbRectangle,
					ps->rectangle.left,
					iThumbPos,
					ps->rectangle.right,
					iThumbPos + iThumbSize);
				::SetRect(&ps->pageRectangle[0],
					ps->rectangle.top,
					ps->arrowRectangle[0].bottom,
					ps->rectangle.right,
					ps->thumbRectangle.top);
				::SetRect(&ps->pageRectangle[1],
					ps->rectangle.top,
					ps->thumbRectangle.top,
					ps->rectangle.right,
					ps->arrowRectangle[1].top);
			}
			else
			{
				::SetRect(&ps->thumbRectangle,
					ps->rectangle.left,
					0,
					ps->rectangle.right,
					0);
				::SetRect(&ps->pageRectangle[0],
					ps->rectangle.top,
					ps->arrowRectangle[0].bottom,
					ps->rectangle.right,
					ps->arrowRectangle[1].top);
				::SetRect(&ps->pageRectangle[1],
					ps->rectangle.top,
					ps->arrowRectangle[0].bottom,
					ps->rectangle.right,
					ps->arrowRectangle[1].top);
			}
		}
		void TinyVisualVScrollBar::ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps)
		{
			ASSERT(ps);
			INT iNewPos = 0;
			INT iRange = (m_si.iMax - m_si.iMin) + 1;
			INT iThumbPos = m_bVertical ? (pt.y - m_si.iThumbOffset) - ps->arrowRectangle[0].bottom : (pt.x - m_si.iThumbOffset) - ps->arrowRectangle[0].right;
			INT iThumbSize = m_bVertical ? TO_CY(ps->thumbRectangle) : TO_CX(ps->thumbRectangle);
			INT iGrooveSize = m_bVertical ? ps->arrowRectangle[1].top - ps->arrowRectangle[0].bottom : (ps->arrowRectangle[1].left - ps->arrowRectangle[0].right);
			if (iThumbPos < 0)
				iThumbPos = 0;
			if (iThumbPos > (iGrooveSize - iThumbSize))
				iThumbPos = iGrooveSize - iThumbSize;
			if (iRange > 0)
			{
				iNewPos = m_si.iMin + MulDiv(iThumbPos, iRange - m_si.iPage, iGrooveSize - iThumbSize);
			}
			if (m_si.iPos != iNewPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(TRUE, SB_THUMBTRACK, iPos, iNewPos);
			}
		}
		void TinyVisualVScrollBar::DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size)
		{
			SCROLLBARCALC si = { 0 };
			ScrollCalculate(&si, size);
			DrawArrow(canvas, &si);
			DrawGroove(canvas, &si);
			DrawThumb(canvas, &si);
		}
		void TinyVisualVScrollBar::DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			TinyRectangle s = this->GetWindowRect();
			TinyRectangle arrowRectangle0 = ps->arrowRectangle[0];
			OffsetRect(&arrowRectangle0, s.left, s.top);
			TinyRectangle arrowRectangle1 = ps->arrowRectangle[1];
			OffsetRect(&arrowRectangle1, s.left, s.top);
			if (m_si.iHitTest == HTSCROLL_LINEUP)
			{
				if (m_si.iHitTestPress == HTSCROLL_LINEUP)
				{
					canvas.DrawImage(*m_images[ARROW1DOWN], arrowRectangle0, m_images[ARROW1DOWN]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
				}
				else
				{
					canvas.DrawImage(*m_images[ARROW1HIGHLIGHT], arrowRectangle0, m_images[ARROW1HIGHLIGHT]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
				}
			}
			else if (m_si.iHitTest == HTSCROLL_LINEDOWN)
			{
				if (m_si.iHitTestPress == HTSCROLL_LINEDOWN)
				{
					canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2DOWN], arrowRectangle1, m_images[ARROW2DOWN]->GetRectangle());
				}
				else
				{
					canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
					canvas.DrawImage(*m_images[ARROW2HIGHLIGHT], arrowRectangle1, m_images[ARROW2HIGHLIGHT]->GetRectangle());
				}
			}
			else
			{
				canvas.DrawImage(*m_images[ARROW1NORMAL], arrowRectangle0, m_images[ARROW1NORMAL]->GetRectangle());
				canvas.DrawImage(*m_images[ARROW2NORMAL], arrowRectangle1, m_images[ARROW2NORMAL]->GetRectangle());
			}
		}
		void TinyVisualVScrollBar::DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			if (m_si.iHitTest == HTSCROLL_THUMB)
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, m_images[SCROLLBARHIGHLIGHT]->GetRectangle());
				srcCenter.top = srcCenter.top + 4;
				srcCenter.bottom = srcCenter.bottom - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.top = dstCenter.top + 4;
				dstCenter.bottom = dstCenter.bottom - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(*m_images[SCROLLBARHIGHLIGHT], thumbRectangle, dstCenter, m_images[SCROLLBARHIGHLIGHT]->GetRectangle(), srcCenter);
			}
			else
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, m_images[SCROLLBARNORMAL]->GetRectangle());
				srcCenter.top = srcCenter.top + 4;
				srcCenter.bottom = srcCenter.bottom - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.top = dstCenter.top + 4;
				dstCenter.bottom = dstCenter.bottom - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(*m_images[SCROLLBARNORMAL], thumbRectangle, dstCenter, m_images[SCROLLBARNORMAL]->GetRectangle(), srcCenter);
			}
		}
		void TinyVisualVScrollBar::DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			RECT dstCenter, srcCenter;
			CopyRect(&srcCenter, m_images[SCROLLBARNORMAL]->GetRectangle());
			srcCenter.top = srcCenter.top + 4;
			srcCenter.bottom = srcCenter.bottom - 4;
			RECT grooveRectangle;
			CopyRect(&grooveRectangle, &ps->rectangle);
			grooveRectangle.top = ps->arrowRectangle[0].bottom;
			grooveRectangle.bottom = ps->arrowRectangle[1].top;
			CopyRect(&dstCenter, &grooveRectangle);
			dstCenter.top = dstCenter.top + 4;
			dstCenter.bottom = dstCenter.bottom - 4;
			TinyRectangle s = this->GetWindowRect();
			OffsetRect(&grooveRectangle, s.left, s.top);
			OffsetRect(&dstCenter, s.left, s.top);
			canvas.DrawImage(*m_images[SCROLLBARNORMAL], grooveRectangle, dstCenter, m_images[SCROLLBARNORMAL]->GetRectangle(), srcCenter);
		}
		void TinyVisualVScrollBar::SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos)
		{
			m_si.iPos = iPos < iMin ? iMin : iPos;
			m_si.iPos = iPos > (iMax - iPage + 1) ? (iMax - iPage + 1) : iPos;
			if (iMin <= m_si.iPos && m_si.iPos <= (iMax - iPage + 1))
			{
				m_si.iMin = iMin;
				m_si.iMax = iMax;
				m_si.iPage = iPage;
			}
			else
			{
				m_si.iPos = m_si.iMin = m_si.iMax = m_si.iMin = 0;
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
		void TinyVisualVScrollBar::SetScrollPos(INT iPos)
		{
			iPos = iPos < m_si.iMin ? m_si.iMin : iPos;
			iPos = iPos > (m_si.iMax - m_si.iPage) ? (m_si.iMax - m_si.iPage + 1) : iPos;
			m_si.iPos = iPos;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
		INT	TinyVisualVScrollBar::GetScrollPos() const
		{
			return m_si.iPos;
		}
		INT	TinyVisualVScrollBar::GetPage() const
		{
			return m_si.iPage;
		}
		INT	TinyVisualVScrollBar::GetMin() const
		{
			return m_si.iMin;
		}
		INT	TinyVisualVScrollBar::GetMax() const
		{
			return m_si.iMax;
		}
		void TinyVisualVScrollBar::SetStyleImage(ScrollImageStyle type, LPCSTR pzFile)
		{
			m_images[type] = TinyVisualResource::GetInstance()[pzFile];
			ASSERT(m_images[type]);
		}

		void TinyVisualVScrollBar::SetSize(const TinySize& size)
		{
			TinyVisual::SetSize(size);
			SetScrollInfo(m_si.iMin, m_si.iMax, size.cy, m_si.iPos);
		}

	}
}
