// GameDetour.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GameDetour.h"
#include "DXGICapture.h"
//#include "DX9Capture.h"
#include "DX8Capture.h"
#include "OpenGLCapture.h"
using namespace DXCapture;

namespace GameDetour
{
	LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			::DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			LOG(INFO) << "WindowProcedure WM_DESTROY\n";
			::PostQuitMessage(0);
			break;
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam); ;
	}
	GameCapture::GameCapture()
		:m_hWNDD3D(NULL),
		m_bDX8Detour(FALSE),
		m_bDX9Detour(FALSE),
		m_bDXGIDetour(FALSE)
	{

	}
	GameCapture::~GameCapture()
	{

	}
	BOOL GameCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		LOG(INFO) << szName << " GameCapture::Attach\n";
		return m_task.Submit(BindCallback(&GameCapture::OnMessagePump, this));
	}
	void GameCapture::Detach(HMODULE hModule)
	{
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		LOG(INFO) << szName << " GameCapture::Detach\n";
		m_bDX9Detour = m_bDXGIDetour = FALSE;
		if (m_hWNDD3D != NULL)
		{
			::PostMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
		}
		m_task.Close(1500);
	}
	void GameCapture::BeginCapture()
	{
		if (g_dx.Initialize())
		{
			LOG(INFO) << "g_dx.Initialize() OK\n";
			if (!m_bDX8Detour)
			{
				LOG(INFO) << "Begin g_dx8.Initialize\n";
				m_bDX8Detour = g_dx8.Initialize(m_hWNDD3D);
			}
			/*if (!m_bDX9Detour)
			{
				LOG(INFO) << "Begin g_dx9.Initialize\n";
				m_bDX9Detour = g_dx9.Initialize(m_hWNDD3D);
			}*/
			if (!m_bDXGIDetour)
			{
				LOG(INFO) << "Begin g_dxgi.Initialize\n";
				m_bDXGIDetour = g_dxgi.Initialize(m_hWNDD3D);
			}
		}
	}
	void GameCapture::EndCapture()
	{
		g_dx.Uninitialize();
		LOG(INFO) << "Uninitialize OK\n";
	}
	void GameCapture::OnMessagePump()
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = (WNDPROC)WindowProcedure;
		wc.lpszClassName = D3D_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			m_hWNDD3D = CreateWindowEx(0,
				D3D_WINDOWCLASS,
				TEXT("D3D Caption Window"),
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
}

