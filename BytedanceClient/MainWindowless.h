#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualWindow.h"
#include "Windowless/TinyVisualButton.h"
#include "Windowless/TinyVisualLabel.h"
#include "Windowless/TinyVisualContextMenu.h"
#include "Windowless/TinyVisualMenuItem.h"
using namespace TinyFramework;
using namespace TinyFramework::Windowless;

namespace Bytedance
{
	/// <summary>
	/// ������
	/// </summary>
	class MainWindowless : public TinyVisualWindowless
	{
		DECLARE_DYNAMIC(MainWindowless)
		DISALLOW_COPY_AND_ASSIGN(MainWindowless)
	public:
		MainWindowless();
		~MainWindowless();
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;

		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;

	private:
		void BuildUI();
		void Resize(INT cx, INT cy);
	private:
		void OnMaxClick(TinyVisual*, EventArgs& args);
		void OnMinClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnRestoreClick(TinyVisual*, EventArgs& args);
		void OnSettingClick(TinyVisual*, EventArgs& args);
	private:
		//UIԪ��
		TinyVisualButton*		m_min;
		TinyVisualButton*		m_max;
		TinyVisualButton*		m_restore;
		TinyVisualButton*		m_close;
		TinyVisualButton*		m_setting;
		TinyVisualContextMenu*	m_contextmenu;
		//�¼�
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMaxClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onRestoreClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSettingClick;
	};
}
