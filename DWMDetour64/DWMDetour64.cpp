// DWMDetour64.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DWMDetour64.h"

namespace DWMDetour64
{
	DWMCapture::DWMCapture()
	{

	}
	DWMCapture::~DWMCapture()
	{

	}

	BOOL DWMCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		return m_task.Submit(BindCallback(&DWMCapture::OnMessagePump, this));
	}

	BOOL DWMCapture::Detach(HMODULE hModule)
	{
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		if (m_hWNDD3D != NULL)
		{
			::PostMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
		}
		return m_task.Close(1500);
	}

	void DWMCapture::BeginCapture()
	{

	}
	void DWMCapture::EndCapture()
	{

	}
	void DWMCapture::OnMessagePump()
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = (WNDPROC)DWMCapture::WindowProcedure;
		wc.lpszClassName = DWM_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			m_hWNDD3D = CreateWindowEx(0,
				DWM_WINDOWCLASS,
				TEXT("DWM Caption Window"),
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				m_hInstance,
				NULL
			);
		}
		if (m_hWNDD3D != NULL)
		{
			BeginCapture();
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			EndCapture();
		}
	}
	LRESULT CALLBACK DWMCapture::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			::DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam); ;
	}
}





