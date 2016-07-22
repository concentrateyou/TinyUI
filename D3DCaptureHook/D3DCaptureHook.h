#pragma once
#include "D3DCommon.h"
#include "D3DCapture.h"
#include "SharedMemory.h"

namespace D3D
{

#define CAPTURE_MEMORY          TEXT("Local\\IQiyiCaptureMemory")
#define TEXTURE_MUTEX1          TEXT("IQiyiTextureMutex1")
#define TEXTURE_MUTEX2          TEXT("IQiyiTextureMutex2")
#define D3D_WINDOWCLASS			TEXT("IQiyiD3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("IQiyiOpenGLGraphicsCapture")
#define IQIYI_WINDOW_CLASS        TEXT("IQiyiWindowClass")
	class CD3DCapture
	{
	public:
		CD3DCapture();
		~CD3DCapture();
	public:
		BOOL Attach(HMODULE hModule);
		BOOL Detach();
	private:
		static DWORD WINAPI CaptureLoop(LPVOID ps);
		static DWORD WINAPI WindowLoop(LPVOID ps);
	private:
		HANDLE			m_hMain;
		HANDLE			m_hCapture;
		HINSTANCE		m_hInstance;
		CSharedMemory	m_memory;
		CMutex			m_mutex1;
		CMutex			m_mutex2;
		D3D9Capture		m_d3d9Capture;
		HWND			m_hWNDMain;//Ö÷´°¿Ú
		HWND			m_hWNDOpenGL;
		HWND			m_hWNDD3D;
	};
}