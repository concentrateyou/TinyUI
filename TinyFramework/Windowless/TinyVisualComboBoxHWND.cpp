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
			:m_offsetX(3),
			m_offsetY(3),
			m_iPos(0),
			m_owner(NULL),
			m_current(NULL)
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
			m_offsetX = 3;
			m_offsetY = 3;
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
			Unpopup();
		}

		void TinyVisualComboBoxHWND::OnUninitialize()
		{

		}

		LRESULT	 TinyVisualComboBoxHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
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
		TinyVisualComboBoxItem*	TinyVisualComboBoxHWND::Add(const TinyString& name, const TinyString& text)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualComboBoxItem* item = static_cast<TinyVisualComboBoxItem*>(m_document.Create(TinyVisualTag::COMBOBOXITEM, window));
			item->SetName(name);
			item->SetText(text);
			item->SetTextColor(0x0000000);
			item->SetTextAlian(DT_CENTER | DT_VCENTER);
			item->SetSize(TinySize(180, 26));
			item->SetPosition(TinyPoint(m_offsetX, m_offsetY));
			item->SetImage(TinyVisualResource::GetInstance()["ComboBoxList_highlight"]);
			item->EVENT_CLICK += m_onItemClick;
			m_offsetY += 26;
			return item;
		}
		void TinyVisualComboBoxHWND::Remove(const TinyString& name)
		{
			TinyVisualComboBoxItem* item = static_cast<TinyVisualComboBoxItem*>(m_document.GetVisualByName(name));
			if (item != NULL)
			{
				item->EVENT_CLICK -= m_onItemClick;
				m_document.Destory(item);
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
		}
		void TinyVisualComboBoxHWND::Popup(const TinyPoint& pos)
		{
			if (!IsPopup())
			{
				TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
				window->SetPosition(pos);
				window->SetSize(TinySize(186, m_offsetY + 3));
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
			//AdjustLayout(0, iOldPos - iNewPos);
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
	}
}
