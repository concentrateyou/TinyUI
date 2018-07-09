#include "../stdafx.h"
#include "TinyVisualComboBoxItem.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualComboBoxHWND.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualComboBoxHWND, TinyVisualWindowless);

		TinyVisualComboBoxHWND::TinyVisualComboBoxHWND()
			:m_count(0),
			m_iPos(0),
			m_owner(NULL),
			m_current(NULL),
			m_itemsize(125, 26)
		{
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualComboBoxHWND::OnPosChange));
			m_onItemClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &TinyVisualComboBoxHWND::OnItemClick));
		}

		TinyVisualComboBoxHWND::~TinyVisualComboBoxHWND()
		{
			m_onItemClick.Reset(NULL);
		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveClassName()
		{
			return TEXT("VISUALCOMBOBOXHWND");
		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveTitle()
		{
			return TEXT("VISUALCOMBOBOXHWND");
		}
		DWORD TinyVisualComboBoxHWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}

		DWORD TinyVisualComboBoxHWND::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}

		void TinyVisualComboBoxHWND::OnInitialize()
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			window->SetBackgroundImage(TinyVisualResource::GetInstance()["ComboBoxList_bkg"]);
			window->SetBackgroundRectangle({ 0,0,69,56 });
			window->SetBackgroundCenter({ 3,3,66,53 });
			m_scrollbar = static_cast<TinyVisualVScrollBar*>(m_document.Create(TinyVisualTag::VSCROLLBAR, window));
			ASSERT(m_scrollbar);
			m_scrollbar->SetVisible(FALSE);
			m_scrollbar->SetImage(ARROW1NORMAL, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_normal"]);
			m_scrollbar->SetImage(ARROW1HIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_hover"]);
			m_scrollbar->SetImage(ARROW1DOWN, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_press"]);
			m_scrollbar->SetImage(ARROW2NORMAL, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"]);
			m_scrollbar->SetImage(ARROW2HIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_hover"]);
			m_scrollbar->SetImage(ARROW2DOWN, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"]);
			m_scrollbar->SetImage(SCROLLBARGROOVE, TinyVisualResource::GetInstance()["vscrollbar_groove"]);
			m_scrollbar->SetImage(SCROLLBARNORMAL, TinyVisualResource::GetInstance()["vscrollbar_normal"]);
			m_scrollbar->SetImage(SCROLLBARHIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_hover"]);
			m_scrollbar->EVENT_POSCHANGE += m_onPosChange;
			Unpopup();
		}

		void TinyVisualComboBoxHWND::OnUninitialize()
		{
			m_scrollbar->EVENT_POSCHANGE -= m_onPosChange;
			m_current = NULL;
		}

		LRESULT	 TinyVisualComboBoxHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return HTCLIENT;
		}
		LRESULT	TinyVisualComboBoxHWND::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == FALSE)
			{
				if (m_owner != NULL)
				{
					m_owner->m_style = TinyVisualComboBox::ComboBoxStyle::NORMAL;
					m_owner->Invalidate();
				}
				::ShowWindow(m_hWND, SW_HIDE);
			}
			return TinyVisualWindowless::OnActivate(uMsg, wParam, lParam, bHandled);;
		}
		LRESULT	TinyVisualComboBoxHWND::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_current != NULL)
			{
				TinyVisualComboBoxItem* current = static_cast<TinyVisualComboBoxItem*>(m_current);
				current->m_dwFlag &= ~ITEM_HIGHLIGHT;
			}
			return TinyVisualWindowless::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}
		TinySize TinyVisualComboBoxHWND::GetItemSize() const
		{
			return m_itemsize;
		}
		void TinyVisualComboBoxHWND::SetItemSize(const TinySize& itemsize)
		{
			m_itemsize = itemsize;
			Invalidate();
		}
		TinyVisualComboBoxItem*	TinyVisualComboBoxHWND::Add(const TinyString& name, const TinyString& text)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualComboBoxItem* item = static_cast<TinyVisualComboBoxItem*>(m_document.Create(TinyVisualTag::COMBOBOXITEM, window));
			item->SetName(name);
			item->SetText(text);
			item->SetTextColor(0x0000000);
			item->SetPadding({ 5,0,0,0 });
			item->SetTextAlian(DT_CENTER | DT_VCENTER);
			item->SetSize(m_itemsize);
			item->SetPosition(TinyPoint(0, m_count * m_itemsize.cy));
			item->SetImage(TinyVisualResource::GetInstance()["ComboBoxList_highlight"]);
			m_count += 1;
			item->EVENT_CLICK += m_onItemClick;
			return item;
		}
		void TinyVisualComboBoxHWND::Remove(const TinyString& name)
		{
			TinyVisualComboBoxItem* item = static_cast<TinyVisualComboBoxItem*>(m_document.GetVisualByName(name));
			if (item != NULL)
			{
				item->EVENT_CLICK -= m_onItemClick;
				m_document.Destory(item);
				m_count -= 1;
			}
		}
		void TinyVisualComboBoxHWND::RemoveAll()
		{
			TinyVisual* spvis = m_document.GetVisual(NULL, CMD_CHILD);
			while (spvis != NULL)
			{
				TinyVisualComboBoxItem* item = static_cast<TinyVisualComboBoxItem*>(spvis);
				m_document.Destory(item);
				spvis = m_document.GetVisual(spvis, CMD_NEXT);
			}
			m_count = 0;
		}
		void TinyVisualComboBoxHWND::Popup(const TinyPoint& pos, INT cy)
		{
			if (!IsPopup())
			{
				TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
				window->SetPosition(pos);
				if (cy > m_itemsize.cy * m_count)
				{
					AdjustItems(m_itemsize.cx);
					window->SetSize(TinySize(m_itemsize.cx, m_itemsize.cy * m_count));
					m_scrollbar->SetVisible(FALSE);
				}
				else
				{
					AdjustItems(m_itemsize.cx - 12);
					window->SetSize(TinySize(m_itemsize.cx, cy));
					m_scrollbar->SetVisible(TRUE);
					m_scrollbar->SetPosition(TinyPoint(m_itemsize.cx - 12, 0));
					m_scrollbar->SetSize(TinySize(12, cy));
					m_scrollbar->SetScrollInfo(0, m_itemsize.cy * (m_count + 1) - cy, m_itemsize.cy, m_iPos);
					m_document.SetFocus(m_scrollbar);
				}
				m_document.Redraw();
				::ShowWindow(m_hWND, SW_SHOW);
			}
		}
		BOOL TinyVisualComboBoxHWND::IsPopup() const
		{
			return ::IsWindowVisible(m_hWND);
		}
		void TinyVisualComboBoxHWND::Unpopup()
		{
			::ShowWindow(m_hWND, SW_HIDE);
		}
		void TinyVisualComboBoxHWND::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			AdjustLayout(0, iOldPos - iNewPos);
			m_iPos = iNewPos;
			m_document.Redraw();
		}
		void TinyVisualComboBoxHWND::OnItemClick(TinyVisual*spvis, EventArgs& args)
		{
			if (m_current != spvis)
			{
				m_current = spvis;
				EVENT_SELECTCHANGED(m_current, args);
				if (m_owner != NULL)
				{
					m_owner->SetText(m_current->GetText());
				}
			}
			Unpopup();
		}
		void TinyVisualComboBoxHWND::AdjustItems(INT cx)
		{
			TinyVisual* spvis = m_document.GetVisual(NULL, CMD_CHILD);
			while (spvis != NULL)
			{
				if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualComboBoxItem)))
				{
					TinySize size = spvis->GetSize();
					size.cx = cx;
					spvis->SetSize(size);
				}
				spvis = m_document.GetVisual(spvis, CMD_NEXT);
			}
		}
		void TinyVisualComboBoxHWND::AdjustLayout(INT dx, INT dy)
		{
			TinyVisual* spvis = m_document.GetVisual(NULL, CMD_CHILD);
			while (spvis != NULL)
			{
				if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualComboBoxItem)))
				{
					TinyRectangle s;
					s.SetPosition(spvis->GetPosition());
					s.SetSize(spvis->GetSize());
					s.OffsetRect(dx, dy);
					spvis->SetPosition(s.Position());
					spvis->SetSize(s.Size());
				}
				spvis = m_document.GetVisual(spvis, CMD_NEXT);
			}
		}
	}
}
