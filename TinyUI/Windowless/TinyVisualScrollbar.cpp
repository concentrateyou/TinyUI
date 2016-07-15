#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualScrollBar.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualScrollBar::TinyVisualScrollBar(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			memset(&m_si, 0, sizeof(SCROLLBOXINFO));
			m_si.iHitTest = m_si.iHitTestPress = HTSCROLL_NONE;
			m_images[0].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_up_normal.png");
			m_images[1].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_up_hover.png");
			m_images[2].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_up_press.png");
			m_images[3].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_down_normal.png");
			m_images[4].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_down_hover.png");
			m_images[5].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\arrow_down_press.png");
			m_images[6].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\scrollbar_groove.png");
			m_images[7].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\scrollbar_normal.png");
			m_images[8].Load("D:\\Github\\TinyUI\\Debug\\Resource\\scrollbar\\scrollbar_hover.png");
		}
		TinyVisualScrollBar::~TinyVisualScrollBar()
		{

		}
		TinyString TinyVisualScrollBar::RetrieveTag() const
		{
			return TinyVisualTag::SCROLLBAR;
		}
		BOOL TinyVisualScrollBar::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			if (canvas.IsValid())
			{
				canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
				TinyRectangle clip = m_vtree->GetWindowRect(this);
				DrawScrollBar(canvas, clip.Size());
			}
			return FALSE;
		}
		HRESULT	TinyVisualScrollBar::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			if (dwFlags & MK_LBUTTON)
			{
				if (m_si.iHitTest == HTSCROLL_THUMB)
				{
					SCROLLBARCALC si = { 0 };
					TinySize size = m_rectangle.Size();
					ScrollCalculate(&si, size);
					ScrollTrackThumb(pos, &si);
					TinyRectangle s = m_vtree->GetWindowRect(this);
					m_vtree->Redraw(&s);
					return TinyVisual::OnMouseMove(pos, dwFlags);
				}
			}
			INT iHitTest = ScrollHitTest(pos);
			if (m_si.iHitTest != iHitTest)
			{
				m_si.iHitTest = iHitTest;
				TinyRectangle s = m_vtree->GetWindowRect(this);
				m_vtree->Redraw(&s);
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	TinyVisualScrollBar::OnMouseLeave()
		{
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualScrollBar::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
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
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			m_vtree->SetCapture(this);
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualScrollBar::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			INT iNewPos = m_si.iPos;
			switch (m_si.iHitTest)
			{
			case HTSCROLL_LINEUP:
				iNewPos = (m_si.iPos - 1) > m_si.iMin ? m_si.iPos - 1 : m_si.iMin;
				break;
			case HTSCROLL_LINEDOWN:
				iNewPos = (m_si.iPos + 1) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + 1 : (m_si.iMax - m_si.iPage + 1);
				break;
			case HTSCROLL_PAGEUP:
				iNewPos = (m_si.iPos - m_si.iPage) > m_si.iMin ? m_si.iPos - m_si.iPage : m_si.iMin;
				break;
			case HTSCROLL_PAGEDOWN:
				iNewPos = (m_si.iPos + m_si.iPage) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + m_si.iPage : (m_si.iMax - m_si.iPage + 1);
				break;
			}
			if (iNewPos != m_si.iPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(iPos, iNewPos);
			}
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			m_si.iHitTest = HTSCROLL_NONE;
			m_si.iHitTestPress = HTSCROLL_NONE;
			m_si.iThumbOffset = 0;
			m_vtree->ReleaseCapture();
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		INT	TinyVisualScrollBar::ScrollHitTest(const TinyPoint& pt)
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
		void TinyVisualScrollBar::ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size)
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
		void TinyVisualScrollBar::ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps)
		{
			ASSERT(ps);
			INT iNewPos = 0;
			INT iRange = (m_si.iMax - m_si.iMin) + 1;
			INT iThumbPos = (pt.y - m_si.iThumbOffset) - ps->arrowRectangle[0].bottom;
			INT iThumbSize = TO_CY(ps->thumbRectangle);
			INT iGrooveSize = ps->arrowRectangle[1].top - ps->arrowRectangle[0].bottom;
			if (iThumbPos < 0)
				iThumbPos = 0;
			if (iThumbPos >(iGrooveSize - iThumbSize))
				iThumbPos = iGrooveSize - iThumbSize;
			if (iRange > 0)
			{
				iNewPos = m_si.iMin + MulDiv(iThumbPos, iRange - m_si.iPage, iGrooveSize - iThumbSize);
			}
			if (m_si.iPos != iNewPos)
			{
				INT iPos = m_si.iPos;
				m_si.iPos = iNewPos;
				EVENT_PosChange(iPos, iNewPos);
			}
		}
		void TinyVisualScrollBar::DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size)
		{
			SCROLLBARCALC si = { 0 };
			ScrollCalculate(&si, size);
			DrawArrow(canvas, &si);
			DrawGroove(canvas, &si);
			DrawThumb(canvas, &si);
		}
		void TinyVisualScrollBar::DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
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
					canvas.DrawImage(m_images[2], arrowRectangle0, m_images[2].GetRectangle());
					canvas.DrawImage(m_images[3], arrowRectangle1, m_images[3].GetRectangle());
				}
				else
				{
					canvas.DrawImage(m_images[1], arrowRectangle0, m_images[1].GetRectangle());
					canvas.DrawImage(m_images[3], arrowRectangle1, m_images[3].GetRectangle());
				}
			}
			else if (m_si.iHitTest == HTSCROLL_LINEDOWN)
			{
				if (m_si.iHitTestPress == HTSCROLL_LINEDOWN)
				{
					canvas.DrawImage(m_images[0], arrowRectangle0, m_images[0].GetRectangle());
					canvas.DrawImage(m_images[5], arrowRectangle1, m_images[5].GetRectangle());
				}
				else
				{
					canvas.DrawImage(m_images[0], arrowRectangle0, m_images[0].GetRectangle());
					canvas.DrawImage(m_images[4], arrowRectangle1, m_images[4].GetRectangle());
				}
			}
			else
			{
				canvas.DrawImage(m_images[0], arrowRectangle0, m_images[0].GetRectangle());
				canvas.DrawImage(m_images[3], arrowRectangle1, m_images[3].GetRectangle());
			}
		}
		void TinyVisualScrollBar::DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			if (m_si.iHitTest == HTSCROLL_THUMB)
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, &m_images[8].GetRectangle());
				srcCenter.top = srcCenter.top + 4;
				srcCenter.bottom = srcCenter.bottom - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.top = dstCenter.top + 4;
				dstCenter.bottom = dstCenter.bottom - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(m_images[8], thumbRectangle, dstCenter, m_images[8].GetRectangle(), srcCenter);
			}
			else
			{
				RECT dstCenter, srcCenter;
				CopyRect(&srcCenter, &m_images[7].GetRectangle());
				srcCenter.top = srcCenter.top + 4;
				srcCenter.bottom = srcCenter.bottom - 4;
				CopyRect(&dstCenter, &ps->thumbRectangle);
				dstCenter.top = dstCenter.top + 4;
				dstCenter.bottom = dstCenter.bottom - 4;
				TinyRectangle s = this->GetWindowRect();
				TinyRectangle thumbRectangle = ps->thumbRectangle;
				OffsetRect(&thumbRectangle, s.left, s.top);
				OffsetRect(&dstCenter, s.left, s.top);
				canvas.DrawImage(m_images[7], thumbRectangle, dstCenter, m_images[7].GetRectangle(), srcCenter);
			}
		}
		void TinyVisualScrollBar::DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps)
		{
			RECT dstCenter, srcCenter;
			CopyRect(&srcCenter, &m_images[7].GetRectangle());
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
			canvas.DrawImage(m_images[7], grooveRectangle, dstCenter, m_images[7].GetRectangle(), srcCenter);
		}
		void TinyVisualScrollBar::SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos)
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
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
		}
		INT	TinyVisualScrollBar::GetScrollPos()
		{
			return m_si.iPos;
		}
	}
}
