// D3DCaptureHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "D3DCaptureHook.h"

namespace D3D
{
	CD3DCapture::CD3DCapture()
		:m_hCapture(NULL),
		m_hWNDOpenGL(NULL),
		m_bD3D8Hook(FALSE),
		m_bD3D9Hook(FALSE),
		m_bD3D10Hook(FALSE),
		m_bD3D11Hook(FALSE)
	{

	}
	CD3DCapture::~CD3DCapture()
	{

	}
	BOOL CD3DCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		if (!(m_hCapture = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CD3DCapture::CaptureLoop, (LPVOID)this, 0, 0)))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL CD3DCapture::Detach()
	{
		if (m_hCapture)
		{
			WaitForSingleObject(m_hCapture, 300);
			CloseHandle(m_hCapture);
			m_hCapture = NULL;
		}
		return TRUE;
	}
	BOOL CD3DCapture::TryCapture()
	{
		if (!m_hWNDD3D || !m_hWNDOpenGL)
			return FALSE;
		if (!m_bD3D9Hook)
		{
			m_bD3D9Hook = D3D9Capture::Instance().Initialize(m_hWNDD3D);
		}
		return TRUE;
	}
	DWORD WINAPI CD3DCapture::CaptureLoop(LPVOID ps)
	{
		CD3DCapture* _this = reinterpret_cast<CD3DCapture*>(ps);
		_this->m_memory.Create(CAPTURE_MEMORY, sizeof(SharedTexture));
		if (!_this->m_memory.Map())
			return FALSE;
		_this->m_hWNDMain = FindWindow(IQIYI_WINDOW_CLASS, NULL);
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = _this->m_hInstance;
		wc.lpfnWndProc = (WNDPROC)DefWindowProc;
		wc.lpszClassName = OPENGL_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			_this->m_hWNDOpenGL = CreateWindowEx(0,
				OPENGL_WINDOWCLASS,
				TEXT("Open GL Capture Window"),
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				_this->m_hInstance,
				NULL
				);
			if (!_this->m_hWNDOpenGL)
				return FALSE;
		}
		wc.lpszClassName = D3D_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			_this->m_hWNDD3D = CreateWindowEx(0,
				OPENGL_WINDOWCLASS,
				TEXT("D3D Caption Window"),
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				_this->m_hInstance,
				NULL
				);
			if (!_this->m_hWNDD3D)
				return FALSE;
		}

		_this->TryCapture();

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return FALSE;
	}
}