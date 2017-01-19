#include "../stdafx.h"
#include "TinyVisualScrollbar.h"
#include "TinyVisualDocument.h"
#include "TinyVisualVBoxLayout.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualVBoxLayout, TinyVisual);
		TinyVisualVBoxLayout::TinyVisualVBoxLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_bAuto(TRUE)
		{

		}
		TinyVisualVBoxLayout::~TinyVisualVBoxLayout()
		{

		}
		TinyString TinyVisualVBoxLayout::RetrieveTag() const
		{
			return TinyVisualTag::VBOXLAYOUT;
		}
		BOOL TinyVisualVBoxLayout::IsLayout() const
		{
			return TRUE;
		}
		void TinyVisualVBoxLayout::AutoScroll(BOOL bAuto)
		{
			m_bAuto = bAuto;
		}
		HRESULT	TinyVisualVBoxLayout::OnCreate()
		{
			ASSERT(m_document);
			TinySize size = this->GetSize();
			m_scrollbar = m_document->Create<TinyVisualVScrollBar>(size.cx - 12, 0, 12, size.cy, this);
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualVBoxLayout::OnPosChange));
			m_scrollbar->EVENT_PosChange += m_onPosChange;
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualVBoxLayout::OnDestory()
		{
			m_scrollbar->EVENT_PosChange -= m_onPosChange;
			return TinyVisual::OnDestory();
		}
		BOOL TinyVisualVBoxLayout::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
		HRESULT	TinyVisualVBoxLayout::OnMouseEnter()
		{
			m_document->SetFocus(m_scrollbar);
			return FALSE;
		}
		HRESULT	TinyVisualVBoxLayout::OnMouseLeave()
		{
			m_document->SetFocus(NULL);
			return FALSE;
		}
		void TinyVisualVBoxLayout::AdjustLayout(TinyVisual* spvis, INT dx, INT dy)
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
		void TinyVisualVBoxLayout::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
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
