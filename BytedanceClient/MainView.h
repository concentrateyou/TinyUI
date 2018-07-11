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
#include "CanvasView.h"

#include "DX9Image2D.h"
#include "DX9RenderView.h"
#include "DX9Graphics2D.h"

using namespace TinyFramework;
using namespace TinyFramework::Windowless;
using namespace DXFramework;

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
		CanvasView				m_canvasView;
		//事件
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMaxClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onRestoreClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSettingClick;

		DX9Image2D				m_image2D;
		DX9Graphics2D			m_graphics;
		DX9RenderView			m_renderView;
	};
}
