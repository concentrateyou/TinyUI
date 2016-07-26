#pragma once
#include "D3DCommon.h"
#include "D3DCapture.h"
#include "SharedMemory.h"

namespace D3D
{

#define CAPTURE_MEMORY          TEXT("Local\\IQiyiCaptureMemory")
#define D3D_WINDOWCLASS			TEXT("IQiyiD3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("IQiyiOpenGLGraphicsCapture")
#define IQIYI_WINDOW_CLASS      TEXT("IQiyiWindowClass")

	class CD3DCapture
	{
	public:
		CD3DCapture();
		~CD3DCapture();
	public:
		BOOL Attach(HMODULE hModule);
		BOOL Detach();
	private:
		BOOL TryCapture();
	private:
		static DWORD WINAPI CaptureLoop(LPVOID ps);
	private:
		HANDLE			m_hCapture;
		HINSTANCE		m_hInstance;
		CSharedMemory	m_memory;
		HWND			m_hWNDMain;
		HWND			m_hWNDOpenGL;
		HWND			m_hWNDD3D;
		BOOL			m_bD3D8Hook;
		BOOL			m_bD3D9Hook;
		BOOL			m_bD3D10Hook;
		BOOL			m_bD3D11Hook;
	};
}