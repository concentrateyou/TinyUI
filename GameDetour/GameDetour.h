#pragma once
#include "Common.h"

namespace GameDetour
{
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define D3D_WINDOWCLASS			TEXT("D3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("OpenGLGraphicsCapture")
#define IQIYI_WINDOW_CLASS      TEXT("QiyiWindowClass")

	class GameCapture
	{
	public:
		GameCapture();
		~GameCapture();
	public:
		BOOL Attach(HMODULE hModule);
		void Detach();
	private:
		BOOL TryCapture();
	private:
		static DWORD WINAPI CaptureTask(LPVOID ps);
	private:
		HANDLE			m_hTask;
		HINSTANCE		m_hInstance;
		HWND			m_hWND;
		HWND			m_hWNDOpenGL;
		HWND			m_hWNDD3D;
		BOOL			m_bDX9Detour;
		BOOL			m_bDXGIDetour;
		HHOOK			m_hHook;
	};
}

