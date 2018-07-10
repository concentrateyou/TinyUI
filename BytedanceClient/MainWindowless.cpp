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

		m_combobox = static_cast<TinyVisualComboBox*>(m_document.Create(TinyVisualTag::COMBOBOX, window));
		ASSERT(m_combobox);
		m_combobox->SetName("Combobox");
		m_combobox->SetTextColor(RGB(0, 0, 0));
		m_combobox->SetTextAlian(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		m_combobox->SetSize(TinySize(129, 28));
		m_combobox->SetPadding({ 5,0,0,0 });
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::NORMAL, TinyVisualResource::GetInstance()["combobox_normal"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::HOVER, TinyVisualResource::GetInstance()["combobox_hover"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::PUSH, TinyVisualResource::GetInstance()["combobox_push"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWDOWN, TinyVisualResource::GetInstance()["inputbtn_down"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWHOVER, TinyVisualResource::GetInstance()["inputbtn_highlight"]);
		m_combobox->SetImage(TinyVisualComboBox::ComboBoxStyle::ARROWNORMAL, TinyVisualResource::GetInstance()["inputbtn_normal"]);

		m_combobox->Add("test1", "上海1");
		m_combobox->Add("test2", "上海2");
		TinyVisualComboBoxItem* item = m_combobox->Add("test3", "上海3");
		m_combobox->SetSelected(item);
		m_combobox->Add("test4", "上海4");
		m_combobox->Add("test5", "上海5");


		//m_scrollbar = static_cast<TinyVisualVScrollBar*>(m_document.Create(TinyVisualTag::VSCROLLBAR, window));
		//m_scrollbar->SetImage(ARROW1NORMAL, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_normal"]);
		//m_scrollbar->SetImage(ARROW1HIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_hover"]);
		//m_scrollbar->SetImage(ARROW1DOWN, TinyVisualResource::GetInstance()["vscrollbar_arrow_up_press"]);
		//m_scrollbar->SetImage(ARROW2NORMAL, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"]);
		//m_scrollbar->SetImage(ARROW2HIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_hover"]);
		//m_scrollbar->SetImage(ARROW2DOWN, TinyVisualResource::GetInstance()["vscrollbar_arrow_down_normal"]);
		//m_scrollbar->SetImage(SCROLLBARGROOVE, TinyVisualResource::GetInstance()["vscrollbar_groove"]);
		//m_scrollbar->SetImage(SCROLLBARNORMAL, TinyVisualResource::GetInstance()["vscrollbar_normal"]);
		//m_scrollbar->SetImage(SCROLLBARHIGHLIGHT, TinyVisualResource::GetInstance()["vscrollbar_hover"]);
		//m_scrollbar->SetPosition(TinyPoint(300, 100));
		//m_scrollbar->SetScrollInfo(0, 300, 100, 0);
		//m_scrollbar->SetSize(TinySize(12, 200));

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
		//TinyPoint pos = m_document.GetScreenPos(spvis);
		//pos.y += spvis->GetSize().cy;
	}
	void MainWindowless::OnRestoreClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		m_restore->SetVisible(FALSE);
		m_max->SetVisible(TRUE);
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
}
