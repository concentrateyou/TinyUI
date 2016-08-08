#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualImageView.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualImageView::TinyVisualImageView(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			m_offsetY = 0;
		}
		TinyVisualImageView::~TinyVisualImageView()
		{

		}
		TinyString TinyVisualImageView::RetrieveTag() const
		{
			return TinyVisualTag::IMAGEVIEW;
		}
		void TinyVisualImageView::SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos)
		{
			m_scrollbar->SetScrollInfo(iMin, iMax, iPage, iPos);
		}
		INT TinyVisualImageView::GetOffertY() const
		{
			return m_offsetY;
		}
		HRESULT	TinyVisualImageView::OnCreate()
		{
			ASSERT(m_document);
			m_image.Load("D:\\test.jpg");
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinySize size = this->GetSize();
			m_scrollbar = static_cast<TinyVisualVScrollBar*>(m_document->GetFactory()->Create(size.cx - 12, 0, 12, size.cy, this, TinyVisualTag::VSCROLLBAR));
			m_scrollbar->SetScrollInfo(0, m_image.GetSize().cy, size.cy, 0);
			m_onPosChange.Reset(new Delegate<void(INT, INT)>(this, &TinyVisualImageView::OnPosChange));
			m_scrollbar->EVENT_PosChange += m_onPosChange;
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualImageView::OnDestory()
		{
			m_scrollbar->EVENT_PosChange -= m_onPosChange;
			return TinyVisual::OnDestory();
		}
		BOOL TinyVisualImageView::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			clip.SetSize(m_image.GetSize());
			clip.right -= 12;
			if (!m_image.IsEmpty())
			{
				TinyMemDC menDC(canvas, m_image);
				TinyRectangle src;
				src.SetRect(0, m_offsetY, m_image.GetSize().cx, m_image.GetSize().cy);
				menDC.Render(clip, src, FALSE);
				//canvas.DrawImage(m_image, clip, 0, 0, m_image.GetSize().cx, m_image.GetSize().cy);
			}
			return TRUE;
		}
		HRESULT	TinyVisualImageView::OnMouseEnter()
		{
			m_document->SetFocus(m_scrollbar);
			return FALSE;
		}
		HRESULT	TinyVisualImageView::OnMouseLeave()
		{
			m_document->SetFocus(NULL);
			return FALSE;
		}
		void TinyVisualImageView::OnPosChange(INT iOldPos, INT iNewPos)
		{
			m_offsetY = iOldPos;
			/*TinyRectangle clip = GetWindowRect();
			INT offset = iOldPos - iNewPos;
			TRACE("offset:%d\n", offset);
			TinyVisualCacheDC* pCacheDC = m_document->GetCacheDC();
			if (pCacheDC)
			{
			HDC hDC = pCacheDC->GetMemDC();
			TinyMemDC cache(hDC, m_image);
			TinyRectangle rect = { 0, 0, cache.GetSize().cx, cache.GetSize().cy };
			BitBlt(hDC, clip.left, clip.top, clip.Width(), clip.Height(), cache.m_hDC, 0, iNewPos, SRCCOPY);
			pCacheDC->Render(clip);
			}*/
			//ScrollWindowEx(m_document->GetVisualHWND()->Handle(), 0, offset, &clip, &clip, NULL, NULL, SW_INVALIDATE);
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
	}
}
