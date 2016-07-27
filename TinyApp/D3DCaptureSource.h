#pragma once
#include "D3DSystem.h"
#include "D3DHook.h"
#include "Common/TinyString.h"
#include "Common/TinyCore.h"
#include "SharedTextureCapture.h"
#include <Windows.h>
#include <TlHelp32.h>

typedef struct tagWNDINFO
{
	HWND	hWND;
	DWORD	dwProcessID;
	DWORD	dwThreadID;
	HANDLE	hProcess;
}WNDINFO, *LPWNDINFO;

#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define APP_EXIT_EVENT          TEXT("AppExit")
#define SHAREDCAPTURE			TEXT("Local\\SharedCapture")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

namespace D3D
{
	class CD3DCaptureSource
	{
	public:
		CD3DCaptureSource();
		~CD3DCaptureSource();
		BOOL BeginCapture(const TinyString& processName);
		BOOL EndCapture();
		void Tick(FLOAT fSeconds);
		void Render(const POINT &pos, const SIZE &size);
	private:
		DWORD	FindProcess(const TinyString& processName);
		BOOL	FindWindow(const TinyString& processName);
		BOOL	Initialize(const TinyString& processName);
		SharedCapture* GetSharedCapture(DWORD processID);
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinyEvent					m_eventBegin;
		TinyEvent					m_eventEnd;
		TinyEvent					m_eventReady;
		TinyEvent					m_eventExit;
		TinySharedMemory			m_sharedCapture;
		WNDINFO						m_targetWND;
		BOOL						m_bCapturing;
		D3D::CSharedTextureCapture	m_sharedTexture;
	};
}

