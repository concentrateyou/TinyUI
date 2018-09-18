// GameDetour.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GameDetour.h"
#include "DXGIGraphicsCapture.h"
#include "DX9GraphicsCapture.h"
#include "DX8GraphicsCapture.h"
#include "OpenGLCapture.h"
using namespace GraphicsCapture;

namespace GameDetour
{
	GameDetour::GameDetour()
		:m_hWNDD3D(NULL),
		m_bDX8Detour(FALSE),
		m_bDX9Detour(FALSE),
		m_bDXGIDetour(FALSE),
		m_hInstance(NULL)
	{

	}
	GameDetour::~GameDetour()
	{

	}
	BOOL GameDetour::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		CHAR szName[MAX_PATH];
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		if (!g_dx.Initialize())
		{
			LOG(ERROR) << "DX Initialize() FAIL\n";
		}
		GetModuleFileName(m_hInstance, szName, MAX_PATH);
		LoadLibrary(szName);
		LOG(INFO) << szName << " GameCapture::Attach\n";
		if (!m_task.Submit(BindCallback(&GameDetour::OnMessagePump, this)))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL GameDetour::Detach(HMODULE hModule)
	{
		CHAR szName[MAX_PATH];
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		m_bDX8Detour = m_bDX9Detour = m_bDXGIDetour = FALSE;
		if (IsWindow(m_hWNDD3D))
		{
			LOG(INFO) << szName << " [GameCapture] GameCapture WM_CLOSE \n";
			::SendMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
			m_task.Close(1000);
		}
		g_dx.Uninitialize();
		m_hWNDD3D = NULL;
		LOG(INFO) << szName << " [GameCapture] Detach\n";
		return TRUE;
	}
	void GameDetour::Detour()
	{
		if (!m_bDX8Detour)
		{
			m_bDX8Detour = g_dx8.Initialize(m_hWNDD3D);
		}
		if (m_bDX8Detour)
		{
			LOG(INFO) << "DX8 Initialize OK\n";
		}
		if (!m_bDX9Detour)
		{
			m_bDX9Detour = g_dx9.Initialize(m_hWNDD3D);
		}
		if (m_bDX9Detour)
		{
			LOG(INFO) << "DX9 Initialize OK\n";
		}
		if (!m_bDXGIDetour)
		{
			m_bDXGIDetour = g_dxgi.Initialize(m_hWNDD3D);
		}
		if (m_bDXGIDetour)
		{
			LOG(INFO) << "DXGI Initialize() OK\n";
		}
	}
	void GameDetour::OnMessagePump()
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = (WNDPROC)GameDetour::WindowProc;
		wc.lpszClassName = D3D_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			m_hWNDD3D = CreateWindowEx(0,
				D3D_WINDOWCLASS,
				TEXT("D3D Window"),
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
			Detour();
			MSG msg = { 0 };
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	LRESULT CALLBACK GameDetour::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			LOG(INFO) << "[GameDetour] WindowProc WM_CLOSE\n";
			::DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			LOG(INFO) << "[GameDetour] WindowProc WM_DESTROY\n";
			::PostQuitMessage(0);
			break;
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam); ;
	}
}

