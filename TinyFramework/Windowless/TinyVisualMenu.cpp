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
			:m_offsetX(7),
			m_offsetY(7)
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
			return (WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}

		DWORD TinyVisualMenu::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}

		TinyVisual* TinyVisualMenu::Add(const TinyString& name, const TinyString& text, TinyImage* image)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(m_document.Create(TinyVisualTag::MENUITEM, window));
			s->SetName(name);
			s->SetText(text);
			s->SetChecked(FALSE);
			s->SetTextColor(0x0000000);
			s->SetTextAlian(DT_LEFT | DT_VCENTER);
			s->SetSize(TinySize(180, 25));
			s->SetPosition(TinyPoint(m_offsetX, m_offsetY));
			s->SetImage(TinyVisualResource::GetInstance()["menu_highlight"], TinyVisualResource::GetInstance()["menu_check"]);
			s->EVENT_CLICK += m_onItemClick;
			m_offsetY += 25;
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
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			window->SetBackgroundImage(TinyVisualResource::GetInstance()["menu_bkg"]);
			window->SetBackgroundRectangle({ 0,0,157,38 });
			window->SetBackgroundCenter({ 7,7,143,24 });
			m_offsetX = 7;
			m_offsetY = 7;
		}

		void TinyVisualMenu::OnUninitialize()
		{

		}

		LRESULT TinyVisualMenu::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return HTCLIENT;
		}

		void TinyVisualMenu::OnItemClick(TinyVisual*spvis, EventArgs& args)
		{
			TinyVisualMenuItem* s = static_cast<TinyVisualMenuItem*>(spvis);
			ASSERT(s);
			s->SetChecked(!s->IsChecked());
			m_document.Invalidate();
		}
		void TinyVisualMenu::Popup(const TinyPoint& pos)
		{
			TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
			window->SetPosition(pos);
			window->SetSize(TinySize(180, m_offsetY));
			m_document.Invalidate();
		}
	}
}
