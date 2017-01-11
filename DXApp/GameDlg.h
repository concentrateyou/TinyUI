#pragma once
#include "Common/TinyDialog.h"
#include "Control/TinyButton.h"
#include "Control/TinyComboBox.h"
using namespace TinyUI;

class GameDlg : public TinyDialog
{
	DECLARE_DYNAMIC(GameDlg)
	BEGIN_MSG_MAP(GameDlg, TinyDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()
public:
	GameDlg();
	virtual ~GameDlg();
public:
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HWND GetGameWND() const;
private:
	static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	void Add(const TinyString& str, HWND hWND);
	void OnRefreshClick(void*, INT);
	void OnGameSelectChange(INT);
private:
	TinyButton		m_refresh;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onRefreshClick;
	TinyComboBox	m_game;
	TinyScopedPtr<Delegate<void(INT)>> m_onGameSelectChange;
	HWND			m_gameWND;
};

