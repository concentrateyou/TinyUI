#include "stdafx.h"
#include "MainView.h"

#include "FLVParser.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(MainView, TinyVisualWindowless);

	MainView::MainView()
		:m_controller(m_view)
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
		BuildUI({ 800,600 });
	}

	void MainView::OnUninitialize()
	{

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
		m_max->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnMaxClick);

		m_min = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_min);
		m_min->SetName("Min");
		m_min->SetSize(TinySize(30, 27));
		m_min->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_min_normal"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_min_hover"]);
		m_min->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_min_down"]);
		m_min->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnMinClick);

		m_close = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_close);
		m_close->SetName("Close");
		m_close->SetSize(TinySize(30, 27));
		m_close->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_close_normal"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_close_hover"]);
		m_close->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_close_down"]);
		m_close->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnCloseClick);

		m_restore = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_restore);
		m_restore->SetVisible(FALSE);
		m_restore->SetName("Restore");
		m_restore->SetSize(TinySize(30, 27));
		m_restore->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["sysbtn_restore_normal"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["sysbtn_restore_hover"]);
		m_restore->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["sysbtn_restore_down"]);
		m_restore->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnRestoreClick);

		m_setting = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_setting);
		m_setting->SetName("Setting");
		m_setting->SetSize(TinySize(30, 27));
		m_setting->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["setting_normal"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["setting_hover"]);
		m_setting->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["setting_down"]);
		m_setting->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnSettingClick);

		m_game = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_game);
		m_game->SetName("btnGame");
		m_game->SetText("ÓÎÏ·×¢Èë");
		m_game->SetSize(TinySize(69, 23));
		m_game->SetTextColor(RGB(0, 0, 0));
		m_game->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_game->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_game->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_game->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);
		m_game->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnGameClick);

		m_monitor = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_monitor);
		m_monitor->SetName("btnMmonitor");
		m_monitor->SetText("ÏÔÊ¾Æ÷²¶»ñ");
		m_monitor->SetSize(TinySize(69, 23));
		m_monitor->SetTextColor(RGB(0, 0, 0));
		m_monitor->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_monitor->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);
		m_monitor->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnMonitorClick);

		m_camera = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_camera);
		m_camera->SetName("btnCamera");
		m_camera->SetText("ÉãÏñÍ·²¶»ñ");
		m_camera->SetSize(TinySize(69, 23));
		m_camera->SetTextColor(RGB(0, 0, 0));
		m_camera->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_camera->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_camera->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_camera->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);
		m_camera->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnCameraClick);

		m_image = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_image);
		m_image->SetName("btnImage");
		m_image->SetText("Ìí¼ÓÍ¼Æ¬");
		m_image->SetSize(TinySize(69, 23));
		m_image->SetTextColor(RGB(0, 0, 0));
		m_image->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_image->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_image->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_image->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);
		m_image->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnImageClick);

		m_media = static_cast<TinyVisualButton*>(m_document.Create(TinyVisualTag::BUTTON, window));
		ASSERT(m_media);
		m_media->SetName("btnMedia");
		m_media->SetText("Ìí¼ÓÊÓÆµ");
		m_media->SetSize(TinySize(69, 23));
		m_media->SetTextColor(RGB(0, 0, 0));
		m_media->SetTextAlian(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_media->SetImage(TinyVisualButton::ButtonStyle::NORMAL, TinyVisualResource::GetInstance()["btn_normal"]);
		m_media->SetImage(TinyVisualButton::ButtonStyle::HOVER, TinyVisualResource::GetInstance()["btn_highlight"]);
		m_media->SetImage(TinyVisualButton::ButtonStyle::DOWN, TinyVisualResource::GetInstance()["btn_down"]);
		m_media->EVENT_CLICK += Delegate<void(TinyVisual*, EventArgs&)>(this, &MainView::OnMediaClick);

		m_native = static_cast<TinyVisualNative*>(m_document.Create(TinyVisualTag::NATIVE, window));
		ASSERT(m_native);
		m_native->SetName("native");
		m_native->SetText("native");
		m_view.Create(m_hWND, 0, 0, 1, 1, FALSE);
		m_controller.Initialize({ 1,1 });
		m_native->SetView(&m_view);

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
			m_camera->SetPosition({ 168,550 });
			m_image->SetPosition({ 248,550 });
			m_media->SetPosition({ 328,550 });

			if (IsWindow(m_view))
			{
				m_native->SetPosition({ 4, m_close->GetSize().cy + 8 });
				m_native->SetSize({ cx - 8, 500 });
			}
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
		m_controller.GetVideoWorker().Stop();
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
		GameVisual2D* visual2D = new GameVisual2D(m_controller.GetVideoWorker().GetDX11());
		//visual2D->Select("Warcraft III","War3.exe","D:\\SourceCode\\TinyUI\\Debug\\GameDetour32.dll");
		visual2D->Select("Direct3DWindowClass", "BasicHLSL11.exe", "D:\\SourceCode\\TinyUI\\Debug\\GameDetour32.dll");
		visual2D->Open();
		TinyRectangle client;
		::GetClientRect(m_view.Handle(), &client);
		visual2D->SetTrackerRect(client);
		m_controller.GetVideoWorker().Add(visual2D);
	}
	void MainView::OnMonitorClick(TinyVisual*, EventArgs& args)
	{
		MonitorVisual2D* visual2D = new MonitorVisual2D(m_controller.GetVideoWorker().GetDX11());
		visual2D->SetOutput(0);
		visual2D->Open();
		TinyRectangle client;
		::GetClientRect(m_view.Handle(), &client);
		XMFLOAT2 size = visual2D->GetSize();
		visual2D->SetTranslate(XMFLOAT2(0.0F, 0.0F));
		visual2D->SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(client)) / size.x, static_cast<FLOAT>(TO_CY(client)) / size.y));
		visual2D->SetTrackerRect(client);
		m_controller.GetVideoWorker().Add(visual2D);
	}
	void MainView::OnCameraClick(TinyVisual*, EventArgs& args)
	{
		CameraVisual2D* visual2D = new CameraVisual2D(m_controller.GetVideoWorker().GetDX11());
		vector<VideoCapture::Name> names;
		VideoCapture::GetDevices(names);
		vector<VideoCaptureFormat> formats;
		VideoCapture::GetDeviceFormats(names[0], formats);
		VideoCaptureParam param;
		for (INT i = 0; i < formats.size(); i++)
		{
			if (formats[i].GetRate() > 15.0F &&
				formats[i].GetSize().cx == 640 &&
				formats[i].GetSize().cy == 360 &&
				(formats[i].GetFormat() == Media::PIXEL_FORMAT_MJPEG ||
					formats[i].GetFormat() == Media::PIXEL_FORMAT_RGB24 ||
					formats[i].GetFormat() == Media::PIXEL_FORMAT_RGB32))
			{
				param.RequestFormat = formats[i];
				break;
			}
		}
		param.RequestFormat.SetFormat(Media::PIXEL_FORMAT_YUY2);
		param.RequestFormat.SetSize({ 640,360 });
		visual2D->Select(names[0], param);
		visual2D->Open();
		TinyRectangle client;
		::GetClientRect(m_view.Handle(), &client);
		XMFLOAT2 size = visual2D->GetSize();
		visual2D->SetTranslate(XMFLOAT2(0.0F, 0.0F));
		visual2D->SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(client)) / size.x, static_cast<FLOAT>(TO_CY(client)) / size.y));
		visual2D->SetTrackerRect(client);
		m_controller.GetVideoWorker().Add(visual2D);
	}
	void MainView::OnMediaClick(TinyVisual*, EventArgs& args)
	{
		vector<LAVVideoFormat> videos;
		vector<LAVAudioFormat> audios;
		StreamVisual2D::QueryFormats("D:\\Ñ¸À×ÏÂÔØ\\BBCµØÆ½Ïß-Ä¾ÐÇ½ÒÃØ\\Ä¾ÐÇ½ÒÃØ.mp4", videos, audios);
		LAVVideoFormat vF;
		LAVAudioFormat aF;
		for (int i = 0; i < videos.size(); i++)
		{
			if (videos[i].GetFormat() == Media::PIXEL_FORMAT_RGB32)
			{
				vF = videos[i];
				break;
			}
		}
		aF = audios[0];
		StreamVisual2D* visual2D = new StreamVisual2D(m_controller.GetVideoWorker().GetDX11());
		visual2D->SetFile("D:\\Ñ¸À×ÏÂÔØ\\BBCµØÆ½Ïß-Ä¾ÐÇ½ÒÃØ\\Ä¾ÐÇ½ÒÃØ.mp4", vF, aF);
		visual2D->Open();
		TinyRectangle client;
		::GetClientRect(m_view.Handle(), &client);
		XMFLOAT2 size = visual2D->GetSize();
		visual2D->SetTranslate(XMFLOAT2(0.0F, 0.0F));
		visual2D->SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(client)) / size.x, static_cast<FLOAT>(TO_CY(client)) / size.y));
		visual2D->SetTrackerRect(client);
		m_controller.GetVideoWorker().Add(visual2D);
	}
	void MainView::OnImageClick(TinyVisual*, EventArgs& args)
	{
		ImageVisual2D*	visual2D = new ImageVisual2D(m_controller.GetVideoWorker().GetDX11());
		visual2D->SetFile("D:\\timg.jpg");
		visual2D->Open();
		TinyRectangle client;
		::GetClientRect(m_view.Handle(), &client);
		XMFLOAT2 size = visual2D->GetSize();
		visual2D->SetTranslate(XMFLOAT2(0.0F, 0.0F));
		visual2D->SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(client)) / size.x, static_cast<FLOAT>(TO_CY(client)) / size.y));
		visual2D->SetTrackerRect(client);
		m_controller.GetVideoWorker().Add(visual2D);
	}
}
