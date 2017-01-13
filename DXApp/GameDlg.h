#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyButton.h"
#include "Control/TinyComboBox.h"
using namespace TinyUI;

namespace DXApp
{
	class GameDlg : public TinyCustomDialog
	{
		DECLARE_DYNAMIC(GameDlg)
	public:
		GameDlg();
		virtual ~GameDlg();

		virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	public:
		virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		HWND GetGameWND() const;
	private:
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
		void Add(const TinyString& str, HWND hWND);
		void OnRefreshClick(void*, INT);
		void OnGameSelectChange(INT);
	private:
		HWND			m_gameWND;
		TinyButton		m_btnRefresh;
		TinyComboBox	m_comboGame;
		TinyScopedPtr<Delegate<void(INT)>> m_onGameSelectChange;
		TinyScopedPtr<Delegate<void(void*, INT)>> m_onRefreshClick;
	};
}



