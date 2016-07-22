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
		DWORD dwProcess = FindProcess("War3.exe");
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
		if (hProcess)
		{
			D3D::InjectLibrary(hProcess, TEXT("D:\\Github\\TinyUI\\Debug\\D3DCaptureHook.dll"));
			CloseHandle(hProcess);
		}
	}

	DWORD TinySkinWindow::FindProcess(const TinyString& name)
	{
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
					break;;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return dwProcessID;
	}
}

