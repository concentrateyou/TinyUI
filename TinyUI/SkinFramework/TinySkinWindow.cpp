#include "../stdafx.h"
#include "TinySkinWindow.h"
#include "../Common/TinyString.h"
#include "../D3D/TinyD3D.h"
#include "../D3D/TinyD3DHook.h"

#include <TlHelp32.h>

namespace TinyUI
{
	TinySkinWindow::TinySkinWindow()
	{

	}

	TinySkinWindow::~TinySkinWindow()
	{
	}

	LRESULT TinySkinWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);//退出应用程序
		return FALSE;
	}

	BOOL TinySkinWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD TinySkinWindow::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD TinySkinWindow::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR TinySkinWindow::RetrieveClassName()
	{
		return TEXT("FramwUI");
	}

	LPCSTR TinySkinWindow::RetrieveTitle()
	{
		return TEXT("FramwUI");
	}

	HICON TinySkinWindow::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT TinySkinWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_inject.Create(m_hWND, 10, 10, 100, 25);
		m_inject.SetText("注入到War3");
		m_onInjectClick.Reset(new Delegate<void(void*, INT)>(this, &TinySkinWindow::OnInjectLibrary));
		m_inject.Click += m_onInjectClick;
		m_lblState.Create(m_hWND, 130, 16, 100, 25);
		m_lblState.SetText("状态");
		m_txtLog.Create(m_hWND, 10, 40, 300, 300);
		m_txtLog.SetReadOnly(TRUE);
		memset(&m_cf, 0, sizeof(m_cf));
		m_cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_EFFECTS;
		m_cf.dwEffects |= CFE_BOLD;
		m_cf.sSpacing = 5;
		m_cf.cbSize = sizeof(CHARFORMAT2);
		m_cf.crTextColor = RGB(0, 114, 193);
		m_cf.yHeight = 180;
		strcpy(m_cf.szFaceName, _T("微软雅黑"));
		return TRUE;
	}

	LRESULT TinySkinWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_inject.Click -= m_onInjectClick;
		return FALSE;
	}

	LRESULT TinySkinWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinySkinWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinySkinWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}

	void TinySkinWindow::OnInjectLibrary(void*, INT)
	{
		SetWindowText(m_txtLog.Handle(), "");
		DWORD dwProcess = FindProcess("War3.exe");

		m_lblState.SetText("War3进程找到");
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
		if (hProcess)
		{
			
			if (D3D::InjectLibrary(hProcess, TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\D3DCaptureHook.dll")))
			{
				m_lblState.SetText("注入成功");
			}
			else
			{
				m_lblState.SetText("注入失败");
			}
			CloseHandle(hProcess);
		}
		else
		{
			m_lblState.SetText("War3进程没找到");
		}
	}

	DWORD TinySkinWindow::FindProcess(const TinyString& name)
	{
		TinyString str1 = "要查找进程";
		str1.Append(name);
		AppendLog(str1);
		DWORD dwProcessID = 0;
		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = { 0 };
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == (HANDLE)-1)
			return FALSE;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				TinyString str(pe32.szExeFile);
				if (str == name)
				{
					dwProcessID = pe32.th32ProcessID;
					break;
				}
				AppendLog(str);
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return dwProcessID;
	}
	void TinySkinWindow::AppendLog(TinyString& str)
	{
		str += "\n";
		INT pos = GetWindowTextLength(m_txtLog.Handle());
		m_txtLog.SetSel(pos, -1);
		m_txtLog.SetSelectionCharFormat(m_cf);
		m_txtLog.ReplaceSel(str.STR());
		::SendMessage(m_txtLog.Handle(), WM_VSCROLL, SB_BOTTOM, 0);
	}
}

