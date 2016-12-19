#include "../stdafx.h"
#include "TinyVisualScrollbar.h"
#include "TinyVisualDocument.h"
#include "TinyVisualHLayout.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualHLayout::TinyVisualHLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_bAuto(FALSE)
		{

		}
		TinyVisualHLayout::~TinyVisualHLayout()
		{

		}
		TinyString TinyVisualHLayout::RetrieveTag() const
		{
			return TinyVisualTag::HORIZONTALLAYOUT;
		}
		void TinyVisualHLayout::AutoScroll(BOOL bAuto)
		{
			m_bAuto = bAuto;
		}
		HRESULT	TinyVisualHLayout::OnCreate()
		{
			ASSERT(m_document);
			TinySize size = this->GetSize();
			m_scrollbar = m_document->Create<TinyVisualHScrollBar>(size.cx - 12, 0, 12, size.cy, this);
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualHLayout::OnPosChange));
			m_scrollbar->EVENT_PosChange += m_onPosChange;
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualHLayout::OnDestory()
		{
			m_scrollbar->EVENT_PosChange -= m_onPosChange;
			return TinyVisual::OnDestory();
		}
		BOOL TinyVisualHLayout::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
		HRESULT	TinyVisualHLayout::OnMouseEnter()
		{
			m_document->SetFocus(m_scrollbar);
			return FALSE;
		}
		HRESULT	TinyVisualHLayout::OnMouseLeave()
		{
			m_document->SetFocus(NULL);
			return FALSE;
		}
		void TinyVisualHLayout::AdjustLayout(TinyVisual* spvis, INT dx, INT dy)
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
		void TinyVisualHLayout::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
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
