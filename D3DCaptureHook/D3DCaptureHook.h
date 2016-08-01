#pragma once
#include "D3DCommon.h"
#include "D3DCapture.h"
#include "SharedMemory.h"

namespace D3D
{
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define D3D_WINDOWCLASS			TEXT("D3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("OpenGLGraphicsCapture")
#define IQIYI_WINDOW_CLASS      TEXT("QiyiWindowClass")

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