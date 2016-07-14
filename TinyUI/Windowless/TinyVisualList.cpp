#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualList.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualList::TinyVisualList(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree),
			m_offsetY(0)
		{

		}
		TinyVisualList::~TinyVisualList()
		{

		}
		TinyString TinyVisualList::RetrieveTag() const
		{
			return TinyVisualTag::LIST;
		}
		HRESULT	TinyVisualList::OnCreate()
		{
			ASSERT(m_vtree);
			TinySize size = this->GetSize();
			m_scrollbar = static_cast<TinyVisualScrollBar*>(m_vtree->GetFactory()->Create(size.cx - 12, 0, 12, size.cy, this, TinyVisualTag::SCROLLBAR));
			m_scrollbar->SetScrollInfo(0, 200, 50, 0);
			m_onPosChange.Reset(new Delegate<void(INT, INT)>(this, &TinyVisualList::OnPosChange));
			m_scrollbar->EVENT_PosChange += m_onPosChange;
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualList::OnDestory()
		{
			m_scrollbar->EVENT_PosChange -= m_onPosChange;
			return TinyVisual::OnDestory();
		}
		BOOL TinyVisualList::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
		void TinyVisualList::AdjustLayout(TinyVisual* spvis, INT dx, INT dy)
		{
			TRACE("AdjustLayout:%d\n", dy);
			while (spvis != NULL)
			{
				TinyRectangle s = spvis->GetRectangle();
				s.OffsetRect(dx, dy);
				spvis->SetPosition(s.Position());
				spvis->SetSize(s.Size());
				spvis = m_vtree->GetVisual(spvis, CMD_PREV);
			}
		}
		void TinyVisualList::OnPosChange(INT iOldPos, INT iNewPos)
		{
			TinyVisual* spvis = m_vtree->GetVisual(this, CMD_CHILD);
			spvis = m_vtree->GetVisual(spvis, CMD_LAST);
			if (spvis == m_scrollbar)
			{
				spvis = m_vtree->GetVisual(m_scrollbar, CMD_PREV);
			}
			AdjustLayout(spvis, 0, iOldPos - iNewPos);
			//更新孩子元素坐标
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
		}
	}
}
