#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "D3D10Device.h"
#include "D3DHook.h"
#include "Common/TinyString.h"
#include "Common/TinyCore.h"
#include "SharedTextureCapture.h"

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
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

namespace D3D
{
	class CD3DCaptureSource
	{
	public:
		explicit CD3DCaptureSource(CD3D10Device& device);
		~CD3DCaptureSource();
		BOOL BeginCapture(const TinyString& processName);
		BOOL EndCapture();
		void Tick(FLOAT fSeconds);
		BOOL Render(const D3DXVECTOR2 &pos, const D3DXVECTOR2 &size);
		BOOL Save(LPCSTR pzFile);
	private:
		BOOL	Initialize(const TinyString& processName);
		DWORD	FindProcess(const TinyString& processName);
		BOOL	FindWindow(const TinyString& processName);
		void	DrawSprite(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2);
		SharedCapture* GetSharedCapture();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		CD3D10Device&				m_device;
		CD3D10Texture				m_targetTexture;
		SharedCapture*				m_pSharedCapture;
		TinyEvent					m_eventBegin;
		TinyEvent					m_eventEnd;
		TinyEvent					m_eventReady;
		TinyEvent					m_eventExit;
		WNDINFO						m_targetWND;
		BOOL						m_bCapturing;
		BOOL						m_bInject;
		D3D::CSharedTextureCapture	m_textureCapture;
		TinySharedMemory			m_sharedCaptureMemory;
	};
}

