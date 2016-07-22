// D3DCaptureHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "D3DCaptureHook.h"

namespace D3D
{
	CD3DCapture::CD3DCapture()
		:m_hMain(NULL),
		m_hCapture(NULL),
		m_hWNDOpenGL(NULL)
	{

	}
	CD3DCapture::~CD3DCapture()
	{

	}
	BOOL CD3DCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		m_hMain = OpenThread(THREAD_ALL_ACCESS, NULL, GetCurrentThreadId());
		if (!(m_hCapture = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CD3DCapture::CaptureLoop, (LPVOID)this, 0, 0)))
		{
			CloseHandle(m_hMain);
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
		}
		return TRUE;
	}
	DWORD WINAPI CD3DCapture::CaptureLoop(LPVOID ps)
	{
		CD3DCapture* _this = reinterpret_cast<CD3DCapture*>(ps);
		if (_this->m_hMain)
		{
			WaitForSingleObject(_this->m_hMain, 150);
			CloseHandle(_this->m_hMain);
		}
		HANDLE hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CD3DCapture::WindowLoop, ps, 0, 0);
		if (!hHandle)
			return FALSE;
		CloseHandle(hHandle);
		_this->m_memory.Create(CAPTURE_MEMORY, sizeof(CaptureEntry));
		if (!_this->m_memory.Map())
			return FALSE;
		_this->m_mutex1.Open(MUTEX_ALL_ACCESS, FALSE, TEXTURE_MUTEX1);
		_this->m_mutex2.Open(MUTEX_ALL_ACCESS, FALSE, TEXTURE_MUTEX2);
		_this->m_hWNDMain = FindWindow(IQIYI_WINDOW_CLASS, NULL);
		return FALSE;
	}
	DWORD WINAPI CD3DCapture::WindowLoop(LPVOID ps)
	{
		CD3DCapture* _this = reinterpret_cast<CD3DCapture*>(ps);
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
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}