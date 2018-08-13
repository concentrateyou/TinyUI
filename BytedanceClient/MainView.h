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


using namespace TinyFramework;
using namespace TinyFramework::Windowless;
namespace Bytedance
{
	/// <summary>
	/// ������
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
	private:
		//UIԪ��
		//������
		TinyVisualButton*		m_min;
		TinyVisualButton*		m_max;
		TinyVisualButton*		m_restore;
		TinyVisualButton*		m_close;
		TinyVisualButton*		m_setting;
		TinyVisualLabel*		m_title;
		TinyVisualNative*		m_native;
		//��ť
		TinyVisualButton*		m_game;//��Ϸע��
		TinyVisualButton*		m_monitor;//��ʾ��
	};
}
