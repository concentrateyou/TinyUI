#pragma once
#include "Common/TinyCommon.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyThread.h"
using namespace TinyFramework;

namespace GameDetour
{
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define D3D_WINDOWCLASS			TEXT("D3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("OpenGLGraphicsCapture")
#define IQIYI_WINDOW_CLASS      TEXT("GameWindowClass")

	class GameCapture
	{
	public:
		GameCapture();
		~GameCapture();
	public:
		BOOL Attach(HMODULE hModule);
		BOOL Detach(HMODULE hModule);
	private:
		void BeginCapture();
		void EndCapture();
		void OnMessagePump();
	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		IO::TinyThread	m_task;
		HINSTANCE			m_hInstance;
		HWND				m_hWNDD3D;
		BOOL				m_bDX8Detour;
		BOOL				m_bDX9Detour;
		BOOL				m_bDXGIDetour;
	};
}

