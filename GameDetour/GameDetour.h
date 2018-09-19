#pragma once
#include "Common/TinyCommon.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyThread.h"
using namespace TinyFramework;

namespace GameDetour
{
#define D3D_WINDOWCLASS			TEXT("D3DGraphicsCapture")
#define OPENGL_WINDOWCLASS		TEXT("OpenGLGraphicsCapture")

	class GameDetour
	{
		DISALLOW_COPY_AND_ASSIGN(GameDetour)
	public:
		GameDetour();
		~GameDetour();
	public:
		BOOL Attach(HMODULE hModule);
		BOOL Detach(HMODULE hModule);
	private:
		BOOL Detour();
		void CaptureLoop();
		void OnMessagePump();
		void OnMessagePumpUI();
	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		BOOL			m_bDX8Detour;
		BOOL			m_bDX9Detour;
		BOOL			m_bDXGIDetour;
		volatile BOOL   m_bStop;
		HWND			m_hWNDD3D;
		HANDLE			m_hMAIN;
		HINSTANCE		m_hInstance;
		IO::TinyThread	m_task;
	};
}

