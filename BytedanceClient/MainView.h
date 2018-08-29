#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualWindow.h"
#include "Windowless/TinyVisualButton.h"
#include "Windowless/TinyVisualLabel.h"
#include "Windowless/TinyVisualContextMenu.h"
#include "Windowless/TinyVisualMenuItem.h"
#include "Windowless/TinyVisualComboBox.h"
#include "Windowless/TinyVisualScrollBar.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualNative.h"

#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
#include "Media/TinyWave.h"

#include "AudioFileSource.h"
#include "AudioFileSink.h"
#include "WaveAudioOutputStream.h"
#include "WaveAudioInputStream.h"
#include "DSAudioOutputStream.h"
#include "DSAudioInputStream.h"
#include "DSFullDuplexAudioInputStream.h"

#include "WASAPIAudioOutputStream.h"
#include "WASAPIAudioInputStream.h"
#include "AudioManager.h"
#include "CanvasView.h"
#include "CanvasController.h"

#include "CameraVisual2D.h"
#include "ImageVisual2D.h"
#include "MonitorVisual2D.h"
#include "GameVisual2D.h"
#include "StreamVisual2D.h"

#include "FLVParser.h"

using namespace TinyFramework;
using namespace TinyFramework::Windowless;
using namespace TinyFramework::Media;
using namespace MediaSDK;
using namespace Decode;

namespace Bytedance
{
	/// <summary>
	/// 主窗口
	/// </summary>
	class MainView : public TinyVisualWindowless
	{
		DECLARE_DYNAMIC(MainView)
		DISALLOW_COPY_AND_ASSIGN(MainView)
	public:
		MainView();
		~MainView();
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void BuildUI(const TinySize& size);
		void Resize(INT cx, INT cy);
	private:
		void OnMaxClick(TinyVisual*, EventArgs& args);
		void OnMinClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnRestoreClick(TinyVisual*, EventArgs& args);
		void OnSettingClick(TinyVisual*, EventArgs& args);
		void OnGameClick(TinyVisual*, EventArgs& args);
		void OnMonitorClick(TinyVisual*, EventArgs& args);
		void OnCameraClick(TinyVisual*, EventArgs& args);
		void OnImageClick(TinyVisual*, EventArgs& args);
	private:
		//UI元素
		//标题栏
		TinyVisualButton*		m_min;
		TinyVisualButton*		m_max;
		TinyVisualButton*		m_restore;
		TinyVisualButton*		m_close;
		TinyVisualButton*		m_setting;
		TinyVisualLabel*		m_title;
		TinyVisualNative*		m_native;
		//按钮
		TinyVisualButton*		m_game;//游戏注入
		TinyVisualButton*		m_monitor;//显示器
		TinyVisualButton*		m_camera;//摄像头
		TinyVisualButton*		m_image;//图片
	private:
		CanvasView				m_view;
		CanvasController		m_controller;
	};
}
