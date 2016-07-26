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
}WNDINFO, *LPWNDINFO;

#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define APP_EXIT_EVENT          TEXT("AppExit")

class CD3DCaptureSource
{
public:
	CD3DCaptureSource();
	~CD3DCaptureSource();
	BOOL Initialize(const TinyString& processName);
	BOOL BeginCapture();
	BOOL EndCapture();

private:
	DWORD	FindProcess(const TinyString& processName);
	HWND	GetGameWindow(DWORD dwProcessID);
	static BOOL CALLBACK CaptureFindWindow(HWND hwnd, LPARAM lParam);
private:
	TinyEvent	m_beginCapture;
	TinyEvent	m_endCapture;
	TinyEvent	m_appExit;
	HANDLE		m_hGameProcess;
	HWND		m_hGameHWND;
	BOOL		m_bCapturing;
	D3D::CSharedTextureCapture	m_sharedTexture;
};

