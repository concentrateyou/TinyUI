#pragma once
#include "Common.h"
#include "IO/TinyTaskBase.h"
using namespace TinyUI;

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
		void Detach(HMODULE hModule);
	private:
		void BeginCapture();
		void EndCapture();
		void OnMessagePump();
	private:
		IO::TinyTaskBase	m_task;
		HINSTANCE			m_hInstance;
		HWND				m_hWNDD3D;
		BOOL				m_bDX9Detour;
		BOOL				m_bDXGIDetour;
	};
}

