#include "stdafx.h"
#include "MainWindowless.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(MainWindowless, TinyVisualWindowless)

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

	}

	void MainWindowless::BuildUI()
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));

		m_max = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		m_max->SetName("Max");
		m_max->SetSize(TinySize(30, 27));
		m_max->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_max_normal"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_max_hover"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_max_down"]);

		m_min = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		m_min->SetName("Min");
		m_min->SetSize(TinySize(30, 27));
		m_min->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_min_normal"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_min_hover"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_min_down"]);

		m_close = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		m_close->SetName("Close");
		m_close->SetSize(TinySize(30, 27));
		m_close->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_close_normal"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_close_hover"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_close_down"]);

		m_restore = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		m_restore->SetName("Restore");
		m_restore->SetSize(TinySize(30, 27));
		m_restore->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_restore_normal"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_restore_hover"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_restore_down"]);

		TinyRectangle s = window->GetClientRect();
		Resize(s.Width(), s.Height());
	}
	void MainWindowless::Resize(INT cx, INT cy)
	{
		INT offset = m_close->GetSize().cx;
		m_close->SetPosition(TinyPoint(cx - offset, 0));
		offset += m_max->GetSize().cx;
		m_max->SetPosition(TinyPoint(cx - offset, 0));
		offset += m_min->GetSize().cx;
		m_min->SetPosition(TinyPoint(cx - offset, 0));
	}
}
