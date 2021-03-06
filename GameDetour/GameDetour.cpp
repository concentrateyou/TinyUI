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
		m_bDX9Hookable(FALSE),
		m_bDXGIDetour(FALSE),
		m_bDX8Hookable(FALSE),
		m_bDX9Detour(FALSE),
		m_bDXGIHookable(FALSE),
		m_bStop(FALSE),
		m_hInstance(NULL),
		m_hMAIN(NULL)
	{

	}
	GameDetour::~GameDetour()
	{

	}
	BOOL GameDetour::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &m_hMAIN, SYNCHRONIZE, FALSE, 0);
		if (!g_dx.Initialize())
		{
			LOG(ERROR) << "DX Initialize FAIL";
		}
		CHAR szName[MAX_PATH];
		GetModuleFileName(m_hInstance, szName, MAX_PATH);
		LoadLibrary(szName);
		if (!m_capture.Submit(BindCallback(&GameDetour::OnMessagePump, this)))
		{
			SAFE_CLOSE_HANDLE(m_hMAIN);
			g_dx.Uninitialize();
			return FALSE;
		}
		LOG(INFO) << "[GameCapture] Attach";
		return TRUE;
	}
	BOOL GameDetour::Detach(HMODULE hModule)
	{
		if (m_capture.IsActive())
		{
			LOG(INFO) << "[GameCapture] Task Close";
			m_bStop = TRUE;
			m_capture.Close(500);
		}
		m_bDX8Detour = m_bDX9Detour = m_bDXGIDetour = FALSE;
		m_bDX8Hookable = m_bDX9Hookable = m_bDXGIHookable = FALSE;
		if (IsWindow(m_hWNDD3D))
		{
			::SendMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
			LOG(INFO) << "[GameCapture] SendMessage WM_CLOSE";
		}
		g_dx.Uninitialize();
		m_hWNDD3D = NULL;
		LOG(INFO) << "[GameCapture] Detach";
		return TRUE;
	}
	void GameDetour::CaptureLoop()
	{
		g_dx.m_sourceReady.WaitEvent(INFINITE);
		while (!Detour())
		{
			Sleep(120);
		}
		for (INT i = 0; !m_bStop; i++)
		{
			if (i % 100 == 0)
			{
				Detour();
			}
			Sleep(120);
		}
	}
	BOOL GameDetour::DetourDX9()
	{
		if (!m_bDX9Detour)
		{
			m_bDX9Detour = g_dx9.Initialize(m_hWNDD3D);
		}
		if (m_bDX9Detour)
		{
			if (!m_bDX9Hookable)
			{
				m_bDX9Hookable = g_dx9.hookable();
			}
			if (m_bDX9Hookable)
			{
				LOG(INFO) << "DX9 Hookable OK";
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL GameDetour::DetourDX8()
	{
		if (!m_bDX8Detour)
		{
			m_bDX8Detour = g_dx8.Initialize(m_hWNDD3D);
		}
		if (m_bDX8Detour)
		{
			if (!m_bDX8Hookable)
			{
				m_bDX8Hookable = g_dx8.hookable();
			}
			if (m_bDX8Hookable)
			{
				LOG(INFO) << "DX8 Hookable OK";
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL GameDetour::DetourDXGI()
	{
		if (!m_bDXGIDetour)
		{
			m_bDXGIDetour = g_dxgi.Initialize(m_hWNDD3D);
		}
		if (m_bDXGIDetour)
		{
			if (!m_bDXGIHookable)
			{
				m_bDXGIHookable = g_dxgi.hookable();
			}
			if (m_bDXGIHookable)
			{
				LOG(INFO) << "DXGI Hookable OK";
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL GameDetour::Detour()
	{
		if (DetourDX9())
			return TRUE;
		if (DetourDXGI())
			return TRUE;
		if (DetourDX8())
			return TRUE;
		return FALSE;
	}
	void GameDetour::OnMessagePumpUI()
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
			MSG msg = { 0 };
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	void GameDetour::OnMessagePump()
	{
		if (m_hMAIN != NULL)
		{
			WaitForSingleObject(m_hMAIN, 250);
			SAFE_CLOSE_HANDLE(m_hMAIN);
		}
		TinyWorker taskUI;
		taskUI.Submit(BindCallback(&GameDetour::OnMessagePumpUI, this));
		g_dx.m_start.SetEvent();
		CaptureLoop();
	}

	LRESULT CALLBACK GameDetour::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

