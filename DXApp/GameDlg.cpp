#include "stdafx.h"
#include "GameDlg.h"
#include "Resource.h"
#include "Utility.h"

namespace DXApp
{

	GameDlg::GameDlg()
		:m_gameWND(NULL)
	{
		m_onRefreshClick.Reset(new Delegate<void(void*, INT)>(this, &GameDlg::OnRefreshClick));
		m_btnRefresh.EVENT_CLICK += m_onRefreshClick;

		m_onGameSelectChange.Reset(new Delegate<void(INT)>(this, &GameDlg::OnGameSelectChange));
		m_comboGame.EVENT_SELECTCHANGE += m_onGameSelectChange;
	}


	GameDlg::~GameDlg()
	{
		m_btnRefresh.EVENT_CLICK -= m_onRefreshClick;
		m_comboGame.EVENT_SELECTCHANGE -= m_onGameSelectChange;
	}


	LRESULT GameDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			if (EndDialog(LOWORD(wParam)))
			{

			}
			break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
	}

	LRESULT GameDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_btnRefresh.SubclassDlgItem(IDC_BTN_REFRESH, m_hWND);

		m_comboGame.SubclassDlgItem(IDC_COMBO_GAME, m_hWND);

		OnRefreshClick(NULL, 0);
		return FALSE;
	}

	void GameDlg::OnRefreshClick(void*, INT)
	{
		m_comboGame.ResetContent();
		EnumWindows(GameDlg::EnumWindow, reinterpret_cast<LPARAM>(this));
	}

	void GameDlg::OnGameSelectChange(INT index)
	{
		m_gameWND = reinterpret_cast<HWND>(m_comboGame.GetItemData(index));
	}
	HWND GameDlg::GetGameWND() const
	{
		return m_gameWND;
	}
	void GameDlg::Add(const TinyString& str, HWND hWND)
	{
		m_comboGame.SetItemData(m_comboGame.AddString(str.STR()), reinterpret_cast<INT_PTR>(hWND));
	}
	BOOL CALLBACK GameDlg::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		GameDlg* dlg = reinterpret_cast<GameDlg*>(lParam);
		if (!IsWindowVisible(hwnd))
			return TRUE;
		DWORD processID;
		GetWindowThreadProcessId(hwnd, &processID);
		if (processID == GetCurrentProcessId())
			return TRUE;
		if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID))
		{
			DWORD size = MAX_PATH;
			CHAR windowExecutable[MAX_PATH];
			if (QueryFullProcessImageName(hProcess, 0, windowExecutable, &size))
			{
				CHAR* pzName = PathFindFileName(windowExecutable);
				TinyArray<TinyString> arrays;
				GetModuleList(hProcess, arrays);
				BOOL bFlag = FALSE;
				for (INT i = 0;i < arrays.GetSize();i++)
				{
					const TinyString& moduleName = arrays[i];
					if (!strcasecmp(moduleName.STR(), TEXT("d3d9.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("d3d10.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("d3d10_1.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("d3d11.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("dxgi.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("d3d8.dll")) ||
						!strcasecmp(moduleName.STR(), TEXT("opengl32.dll")))
					{
						bFlag = TRUE;
						break;
					}
				}
				if (bFlag)
				{
					TinyString windowName;
					windowName.Resize(GetWindowTextLength(hwnd) + 1);
					GetWindowText(hwnd, windowName.STR(), windowName.GetSize());
					TinyString str;
					str.Append("[");
					str.Append(pzName);
					str.Append("] ");
					str.Append(windowName);
					dlg->Add(str, hwnd);
				}
			}
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return TRUE;
	}
}