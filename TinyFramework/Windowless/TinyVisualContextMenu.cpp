#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualContextMenu.h"
#include "TinyVisualMenuItem.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualContextMenu, TinyVisualWindowless);

		TinyVisualContextMenu::TinyVisualContextMenu()
			:m_offsetX(6),
			m_offsetY(6),
			m_owner(NULL)
		{
			m_onItemClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &TinyVisualContextMenu::OnItemClick));
		}

		TinyVisualContextMenu::~TinyVisualContextMenu()
		{
			m_onItemClick.Reset(NULL);
		}
		LPCSTR TinyVisualContextMenu::RetrieveClassName()
		{
			return TEXT("VISUALCONTEXTMENU");
		}
		LPCSTR TinyVisualContextMenu::RetrieveTitle()
		{
			return TEXT("VISUALCONTEXTMENU");
		}
		DWORD TinyVisualContextMenu::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}

		DWORD TinyVisualContextMenu::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}

		TinyVisualMenuItem* TinyVisualContextMenu::Add()
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
		TinyVisualMenuItem* TinyVisualContextMenu::Add(const TinyString& name, const TinyString& text, TinyImage* icon)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualMenuItem* item = static_cast<TinyVisualMenuItem*>(m_document.Create(TinyVisualTag::MENUITEM, window));
			item->SetName(name);
			item->SetText(text);
			item->SetChecked(FALSE);
			item->SetTextColor(0x0000000);
			item->SetTextAlian(DT_LEFT | DT_VCENTER);
			item->SetSize(TinySize(180, 26));
			item->SetPosition(TinyPoint(m_offsetX, m_offsetY));
			item->SetImageList(icon,
				TinyVisualResource::GetInstance()["menu_highlight"],
				TinyVisualResource::GetInstance()["menu_check"],
				TinyVisualResource::GetInstance()["menu_arrow"]);
			item->EVENT_CLICK += m_onItemClick;
			m_offsetY += 26;
			return item;
		}

		void TinyVisualContextMenu::Remove(const TinyString& name)
		{
			TinyVisualMenuItem* item = static_cast<TinyVisualMenuItem*>(m_document.GetVisualByName(name));
			if (item != NULL)
			{
				item->EVENT_CLICK -= m_onItemClick;
				item->RemoveAll();
				m_document.Destory(item);
			}
		}

		void TinyVisualContextMenu::RemoveAll()
		{
			TinyVisual* spvis = m_document.GetVisual(NULL, CMD_CHILD);
			while (spvis != NULL)
			{
				TinyVisualMenuItem* item = static_cast<TinyVisualMenuItem*>(spvis);
				item->EVENT_CLICK -= m_onItemClick;
				item->RemoveAll();
				spvis = m_document.GetVisual(spvis, CMD_NEXT);
			}
		}

		void TinyVisualContextMenu::OnInitialize()
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

		void TinyVisualContextMenu::OnUninitialize()
		{
			RemoveAll();
		}

		LRESULT TinyVisualContextMenu::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return HTCLIENT;
		}
		LRESULT TinyVisualContextMenu::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				CHAR names[256];
				::GetClassName((HWND)lParam, names, 256);
				if (strcasecmp(names, "VISUALCONTEXTMENU") != 0)
				{
					TinyVisualContextMenu* context = m_owner == NULL ? this : m_owner;
					for (;;)
					{
						if (context->m_owner != NULL)
						{
							context = context->m_owner;
							continue;
						}
						context->Unpopup();
						break;
					}
				}
			}
			return TinyVisualWindowless::OnActivate(uMsg, wParam, lParam, bHandled);;
		}
		void TinyVisualContextMenu::OnItemClick(TinyVisual*spvis, EventArgs& args)
		{
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(spvis);
			TinyVisualContextMenu* context = m_owner == NULL ? this : m_owner;
			for (;;)
			{
				if (context->m_owner != NULL)
				{
					context = context->m_owner;
					continue;
				}
				context->Unpopup();
				break;
			}
		}
		void TinyVisualContextMenu::Popup(const TinyPoint& pos)
		{
			if (!IsPopup())
			{
				TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
				window->SetPosition(pos);
				window->SetSize(TinySize(192, m_offsetY + 6));
				m_document.Redraw();
				::ShowWindow(m_hWND, SW_SHOW);
			}
		}
		BOOL TinyVisualContextMenu::IsPopup() const
		{
			return ::IsWindowVisible(m_hWND);
		}
		void TinyVisualContextMenu::Unpopup()
		{
			for (INT i = 0; i < m_contexts.GetSize(); i++)
			{
				TinyVisualContextMenu* s = m_contexts[i];
				s->Unpopup();
			}
			::ShowWindow(m_hWND, SW_HIDE);
		}
	}
}
