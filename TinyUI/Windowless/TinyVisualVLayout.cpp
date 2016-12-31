#include "../stdafx.h"
#include "TinyVisualScrollbar.h"
#include "TinyVisualDocument.h"
#include "TinyVisualVLayout.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualVLayout::TinyVisualVLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_bAuto(TRUE)
		{

		}
		TinyVisualVLayout::~TinyVisualVLayout()
		{

		}
		TinyString TinyVisualVLayout::RetrieveTag() const
		{
			return TinyVisualTag::VERTICALLAYOUT;
		}
		BOOL TinyVisualVLayout::IsLayout() const
		{
			return TRUE;
		}
		void TinyVisualVLayout::AutoScroll(BOOL bAuto)
		{
			m_bAuto = bAuto;
		}
		void TinyVisualVLayout::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
		{
			m_horizontalAlignment = horizontalAlignment;
		}
		void TinyVisualVLayout::SetVerticalAlignment(VerticalAlignment verticalAlignment)
		{
			m_verticalAlignment = verticalAlignment;
		}
		HRESULT	TinyVisualVLayout::OnCreate()
		{
			ASSERT(m_document);
			TinySize size = this->GetSize();
			m_scrollbar = m_document->Create<TinyVisualVScrollBar>(size.cx - 12, 0, 12, size.cy, this);
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualVLayout::OnPosChange));
			m_scrollbar->EVENT_PosChange += m_onPosChange;
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualVLayout::OnDestory()
		{
			m_scrollbar->EVENT_PosChange -= m_onPosChange;
			return TinyVisual::OnDestory();
		}
		BOOL TinyVisualVLayout::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
		HRESULT	TinyVisualVLayout::OnMouseEnter()
		{
			m_document->SetFocus(m_scrollbar);
			return FALSE;
		}
		HRESULT	TinyVisualVLayout::OnMouseLeave()
		{
			m_document->SetFocus(NULL);
			return FALSE;
		}
		void TinyVisualVLayout::AdjustLayout(TinyVisual* spvis, INT dx, INT dy)
		{
			while (spvis != NULL)
			{
				TinyRectangle s = spvis->GetRectangle();
				s.OffsetRect(dx, dy);
				spvis->SetPosition(s.Position());
				spvis->SetSize(s.Size());
				spvis = m_document->GetVisual(spvis, CMD_PREV);
			}
		}
		void TinyVisualVLayout::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			TinyVisual* spvis = m_document->GetVisual(this, CMD_CHILD);
			spvis = m_document->GetVisual(spvis, CMD_LAST);
			if (spvis == m_scrollbar)
			{
				spvis = m_document->GetVisual(m_scrollbar, CMD_PREV);
			}
			AdjustLayout(spvis, 0, iOldPos - iNewPos);
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Invalidate(&s);
		}
	}
}
