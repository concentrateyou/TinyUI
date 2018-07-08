#include "stdafx.h"
#include "MainWindowless.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(MainWindowless, TinyVisualWindowless);

	MainWindowless::MainWindowless()
	{
	}

	MainWindowless::~MainWindowless()
	{
	}

	void MainWindowless::OnInitialize()
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
		window->SetMinimumSize(TinySize(800, 600));
		window->SetSize(TinySize(800, 600));
		window->SetPosition(TinySize(100, 100));
		BuildUI();
	}

	void MainWindowless::OnUninitialize()
	{
		m_max->EVENT_CLICK -= m_onMaxClick;
		m_min->EVENT_CLICK -= m_onMaxClick;
		m_close->EVENT_CLICK += m_onCloseClick;
		m_restore->EVENT_CLICK -= m_onRestoreClick;
		m_setting->EVENT_CLICK -= m_onSettingClick;

		/*if (m_contextmenu != NULL)
		{
			m_contextmenu->DestroyWindow();
			SAFE_DELETE(m_contextmenu);
		}*/
	}

	LRESULT MainWindowless::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (lParam != NULL)
		{
			TinySize size;
			size.cx = LOWORD(lParam);
			size.cy = HIWORD(lParam);
			Resize(size.cx, size.cy);
		}
		return TinyVisualWindowless::OnSize(uMsg, wParam, lParam, bHandled);
	}

	void MainWindowless::BuildUI()
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));

		m_max = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_max);
		m_max->SetName("Max");
		m_max->SetSize(TinySize(30, 27));
		m_max->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_max_normal"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_max_hover"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_max_down"]);
		m_onMaxClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MainWindowless::OnMaxClick));
		m_max->EVENT_CLICK += m_onMaxClick;

		m_min = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_min);
		m_min->SetName("Min");
		m_min->SetSize(TinySize(30, 27));
		m_min->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_min_normal"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_min_hover"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_min_down"]);
		m_onMinClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MainWindowless::OnMinClick));
		m_min->EVENT_CLICK += m_onMinClick;

		m_close = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_close);
		m_close->SetName("Close");
		m_close->SetSize(TinySize(30, 27));
		m_close->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_close_normal"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_close_hover"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_close_down"]);
		m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MainWindowless::OnCloseClick));
		m_close->EVENT_CLICK += m_onCloseClick;

		m_restore = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_restore);
		m_restore->SetVisible(FALSE);
		m_restore->SetName("Restore");
		m_restore->SetSize(TinySize(30, 27));
		m_restore->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_restore_normal"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_restore_hover"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_restore_down"]);
		m_onRestoreClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MainWindowless::OnRestoreClick));
		m_restore->EVENT_CLICK += m_onRestoreClick;

		m_setting = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_setting);
		m_setting->SetName("Setting");
		m_setting->SetSize(TinySize(30, 27));
		m_setting->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["setting_normal"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["setting_hover"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["setting_down"]);
		m_onSettingClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MainWindowless::OnSettingClick));
		m_setting->EVENT_CLICK += m_onSettingClick;

		/*m_contextmenu = new TinyVisualContextMenu();
		m_contextmenu->Create(m_hWND, "");
		TinyVisualMenuItem* item = m_contextmenu->Add("system_setting1", "系统设置1");
		item->Add("system_setting1-1", "系统设置1-1");
		item->Add("system_setting1-2", "系统设置1-2");
		item->Add();
		TinyVisualMenuItem* item1 = item->Add("system_setting1-3", "系统设置1-3");
		item1->Add("system_setting1-1-1", "系统设置1-1-1");
		item1->Add("system_setting1-1-2", "系统设置1-1-2");
		item1->Add();
		item1->Add("system_setting1-1-3", "系统设置1-1-3");
		m_contextmenu->Add("system_setting2", "系统设置2");
		m_contextmenu->Add();
		m_contextmenu->Add("system_setting3", "系统设置3");
		m_contextmenu->Add("system_setting4", "系统设置4");
		m_contextmenu->Add("system_setting5", "系统设置5");*/

		m_combobox = static_cast<TinyVisualComboBox*>(m_document.Create(TinyVisualTag::COMBOBOX, window));
		ASSERT(m_combobox);
		m_combobox->SetName("Combobox");
		m_combobox->SetText("测试");
		m_combobox->SetTextColor(RGB(0, 0, 0));
		m_combobox->SetTextAlian(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		m_combobox->SetSize(TinySize(129, 28));
		m_combobox->SetPadding({ 5,0,0,0 });
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::NORMAL, TinyVisualResource::GetInstance()["combobox_normal"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::HOVER, TinyVisualResource::GetInstance()["combobox_hover"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::DOWN, TinyVisualResource::GetInstance()["combobox_push"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWDOWN, TinyVisualResource::GetInstance()["inputbtn_down"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWHOVER, TinyVisualResource::GetInstance()["inputbtn_highlight"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWNORMAL, TinyVisualResource::GetInstance()["inputbtn_normal"]);

		TinyRectangle s = window->GetClientRect();
		Resize(s.Width(), s.Height());
	}
	void MainWindowless::Resize(INT cx, INT cy)
	{
		if (m_close != NULL)
		{
			INT offset = m_close->GetSize().cx;
			m_close->SetPosition(TinyPoint(cx - offset, 0));
			offset += m_max->GetSize().cx;
			m_max->SetPosition(TinyPoint(cx - offset, 0));
			m_restore->SetPosition(TinyPoint(cx - offset, 0));
			offset += m_min->GetSize().cx;
			m_min->SetPosition(TinyPoint(cx - offset, 0));
			offset += m_setting->GetSize().cx;
			m_setting->SetPosition(TinyPoint(cx - offset, 0));
		}
	}

	void MainWindowless::OnMaxClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		m_restore->SetVisible(TRUE);
		m_max->SetVisible(FALSE);
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
	}
	void MainWindowless::OnMinClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}
	void MainWindowless::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_CLOSE, NULL);
	}
	void MainWindowless::OnSettingClick(TinyVisual* spvis, EventArgs& args)
	{
		TinyPoint pos = m_document.GetScreenPos(spvis);
		pos.y += spvis->GetSize().cy;
		//m_contextmenu->Popup(pos);
	}
	void MainWindowless::OnRestoreClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		m_restore->SetVisible(FALSE);
		m_max->SetVisible(TRUE);
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
}
