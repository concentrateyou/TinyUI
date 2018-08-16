#include "stdafx.h"
#include "MainView.h"

#include "FLVParser.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(MainView, TinyVisualWindowless);

	MainView::MainView()
	{
	}

	MainView::~MainView()
	{
	}

	void MainView::OnInitialize()
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
		window->SetMinimumSize(TinySize(800, 600));
		window->SetSize(TinySize(800, 600));
		window->SetPosition(TinySize(100, 100));
		//m_canvasView.Create(m_hWND, 0, 0, 1, 1, FALSE);
		BuildUI({ 800,600 });

		/*	m_source.Open("D:\\test2.wav");
			AudioParameters ap;
			ap.SetFrames(1024);
			ap.SetFormat(m_source.GetFormat());
			std::vector<PLAYDEVICE> devices;
			TinySoundPlayer::Enumerate(devices);
			m_stream.Initialize(ap, 3, GetDesktopWindow(), devices[0].Guid);
			m_stream.Open();
			m_stream.Start(&m_source);*/
			/*m_capture.Initialize(BindCallback(&MainView::OnCallback1, this));
			std::vector<TinyWASAPIAudio::Name> names;
			m_capture.GetDevices(eAll, names);
			m_capture.Open(names[0], NULL);
			m_waveFile.Create("D:\\test.wav", m_capture.GetFormat());
			m_capture.Start();*/
			/*m_waveFile.Open("D:\\test1.wav");
			m_render.Initialize(BindCallback(&MainView::OnCallback2, this));
			std::vector<TinyWASAPIAudio::Name> names;
			m_render.GetDevices(eAll, names);
			m_render.Open(names[0], m_waveFile.GetFormat());
			m_render.Start();*/

		AudioParameters ap;
		ap.SetFrames(1024);
		m_sink.Create(ap, "D:\\test3.wav");
		std::vector<PLAYDEVICE> devices1;
		TinySoundPlayer::Enumerate(devices1);
		std::vector<CAPTUREDEVICE> devices2;
		TinySoundCapture::Enumerate(devices2);
		m_stream.Initialize(ap, 3, GetDesktopWindow(), devices2[0].Guid, devices1[0].Guid);
		m_stream.Open();
		m_stream.Start(&m_sink);
	}

	void MainView::OnUninitialize()
	{
		m_waveFile.Close();
		//m_capture.Stop();
		//m_capture.Close();
	}

	LRESULT MainView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	void MainView::BuildUI(const TinySize& size)
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));

		m_max = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_max);
		m_max->SetName("Max");
		m_max->SetSize(TinySize(30, 27));
		m_max->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_max_normal"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_max_hover"]);
		m_max->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_max_down"]);

		m_min = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_min);
		m_min->SetName("Min");
		m_min->SetSize(TinySize(30, 27));
		m_min->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_min_normal"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_min_hover"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_min_down"]);

		m_close = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_close);
		m_close->SetName("Close");
		m_close->SetSize(TinySize(30, 27));
		m_close->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_close_normal"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_close_hover"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_close_down"]);

		m_restore = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_restore);
		m_restore->SetVisible(FALSE);
		m_restore->SetName("Restore");
		m_restore->SetSize(TinySize(30, 27));
		m_restore->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_restore_normal"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_restore_hover"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_restore_down"]);

		m_setting = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_setting);
		m_setting->SetName("Setting");
		m_setting->SetSize(TinySize(30, 27));
		m_setting->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["setting_normal"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["setting_hover"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["setting_down"]);

		m_game = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_game);
		m_game->SetName("btnGame");
		m_game->SetText("游戏注入");
		m_game->SetSize(TinySize(69, 23));
		m_game->SetTextColor(RGB(0, 0, 0));
		m_game->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_game->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_game->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_game->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);

		m_monitor = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_monitor);
		m_monitor->SetName("btnMmonitor");
		m_monitor->SetText("显示器捕获");
		m_monitor->SetSize(TinySize(69, 23));
		m_monitor->SetTextColor(RGB(0, 0, 0));
		m_monitor->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);

		TinyRectangle s = window->GetClientRect();
		Resize(s.Width(), s.Height());


	}
	void MainView::Resize(INT cx, INT cy)
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

			m_game->SetPosition({ 8,550 });

			m_monitor->SetPosition({ 88,550 });
		}
	}

	void MainView::OnMaxClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		m_restore->SetVisible(TRUE);
		m_max->SetVisible(FALSE);
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
	}
	void MainView::OnMinClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}
	void MainView::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_CLOSE, NULL);
	}
	void MainView::OnSettingClick(TinyVisual* spvis, EventArgs& args)
	{

		//TinyPoint pos = m_document.GetScreenPos(spvis);
		//pos.y += spvis->GetSize().cy;
	}
	void MainView::OnRestoreClick(TinyVisual*, EventArgs& args)
	{
		m_document.ReleaseCapture();
		m_restore->SetVisible(FALSE);
		m_max->SetVisible(TRUE);
		::SendMessage(m_hWND, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
	void MainView::OnGameClick(TinyVisual*, EventArgs& args)
	{

	}
	void MainView::OnMonitorClick(TinyVisual*, EventArgs& args)
	{
		/*	Decode::FLVParser parser;
			parser.Open("D:\\no_stall.flv");
			parser.Parse();
			parser.Close();*/
	}
	void MainView::OnCallback1(BYTE* bits, LONG count, LPVOID)
	{
		/*WAVEFORMATEX* pFMT = m_capture.GetFormat();
		if (bits != 0)
		{
			m_waveFile.Write(bits, count * pFMT->nBlockAlign);
		}
		else
		{
			TinyScopedArray<BYTE> data(new BYTE[count * pFMT->nBlockAlign]);
			ZeroMemory(data, count * pFMT->nBlockAlign);
			m_waveFile.Write(data, count * pFMT->nBlockAlign);
		}*/
	}
	void MainView::OnCallback2(BYTE* bits, LONG size, LPVOID)
	{
		LONG numberOfBytesRead = 0;
		m_waveFile.Read(bits, size, &numberOfBytesRead);
	}
}
