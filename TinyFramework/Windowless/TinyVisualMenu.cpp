#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualMenu.h"
#include "TinyVisualMenuItem.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualMenu, TinyVisualWindowless);

		TinyVisualMenu::TinyVisualMenu()
			:m_offsetX(6),
			m_offsetY(6)
		{
			m_onItemClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &TinyVisualMenu::OnItemClick));
		}

		TinyVisualMenu::~TinyVisualMenu()
		{
			m_onItemClick.Reset(NULL);
		}
		LPCSTR TinyVisualMenu::RetrieveClassName()
		{
			return TEXT("TinyVisualMenu");
		}
		LPCSTR TinyVisualMenu::RetrieveTitle()
		{
			return TEXT("TinyVisualMenu");
		}
		DWORD TinyVisualMenu::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}

		DWORD TinyVisualMenu::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}
		TinyVisual* TinyVisualMenu::Add()
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(m_document.Create(TinyVisualTag::MENUITEM, window));
			s->SetEnable(FALSE);
			s->SetSeparator(TRUE);
			s->SetSize(TinySize(180, 3));
			s->SetPosition(TinyPoint(m_offsetX, m_offsetY));
			s->SetBackgroundImage(TinyVisualResource::GetInstance()["menu_cutling"]);
			s->SetBackgroundRectangle({ 0,0,50,3 });
			s->SetBackgroundCenter({ 1,0,49,3 });
			s->SetImageList(NULL,
				TinyVisualResource::GetInstance()["menu_highlight"],
				TinyVisualResource::GetInstance()["menu_check"],
				TinyVisualResource::GetInstance()["menu_arrow"]);
			m_offsetY += 3;
			return s;
		}
		TinyVisual* TinyVisualMenu::Add(const TinyString& name, const TinyString& text, TinyImage* icon)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(m_document.Create(TinyVisualTag::MENUITEM, window));
			s->SetName(name);
			s->SetText(text);
			s->SetChecked(FALSE);
			s->SetTextColor(0x0000000);
			s->SetTextAlian(DT_LEFT | DT_VCENTER);
			s->SetSize(TinySize(180, 26));
			s->SetPosition(TinyPoint(m_offsetX, m_offsetY));
			s->SetBackgroundImage(TinyVisualResource::GetInstance()["menu_bkg"]);
			s->SetBackgroundRectangle({ 6,6,151,32 });
			s->SetImageList(icon,
				TinyVisualResource::GetInstance()["menu_highlight"],
				TinyVisualResource::GetInstance()["menu_check"],
				TinyVisualResource::GetInstance()["menu_arrow"]);
			s->EVENT_CLICK += m_onItemClick;
			m_offsetY += 26;
			return s;
		}

		void TinyVisualMenu::Remove(const TinyString& name)
		{
			TinyVisual* s = m_document.GetVisualByName(name);
			if (s != NULL)
			{
				s->EVENT_CLICK -= m_onItemClick;
			}
		}

		void TinyVisualMenu::OnInitialize()
		{
			AllowTracking(TRUE);
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			window->SetBackgroundImage(TinyVisualResource::GetInstance()["menu_bkg"]);
			window->SetBackgroundRectangle({ 0,0,157,38 });
			window->SetBackgroundCenter({ 6,6,151,32 });
			m_offsetX = 6;
			m_offsetY = 6;
			Unpopup();
		}

		void TinyVisualMenu::OnUninitialize()
		{

		}

		LRESULT TinyVisualMenu::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return HTCLIENT;
		}
		LRESULT TinyVisualMenu::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			Unpopup();
			return TinyVisualWindowless::OnMouseLeave(uMsg, wParam, lParam, bHandled);
		}
		LRESULT TinyVisualMenu::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				TinyString className(256);
				::GetClassName((HWND)lParam, className.STR(), 256);
				if (!className.Compare("TinyVisualMenu"))
				{
					Unpopup();
				}
			}
			return TinyVisualWindowless::OnActivate(uMsg, wParam, lParam, bHandled);;
		}
		void TinyVisualMenu::OnItemClick(TinyVisual*spvis, EventArgs& args)
		{
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(spvis);
			ASSERT(s);
			s->SetChecked(!s->IsChecked());
		}
		void TinyVisualMenu::Popup(const TinyPoint& pos)
		{
			if (!IsPopup())
			{
				TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
				window->SetPosition(pos);
				window->SetSize(TinySize(192, m_offsetY + 6));
				::ShowWindow(m_hWND, SW_SHOWDEFAULT);
				m_document.Invalidate();
			}
		}
		BOOL TinyVisualMenu::IsPopup() const
		{
			return ::IsWindowVisible(m_hWND);
		}
		void TinyVisualMenu::Unpopup()
		{
			::ShowWindow(m_hWND, SW_HIDE);
		}
	}
}
