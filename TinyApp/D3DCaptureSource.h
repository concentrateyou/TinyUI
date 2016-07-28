#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "D3DDevice.h"
#include "D3DHook.h"
#include "Common/TinyString.h"
#include "Common/TinyCore.h"
#include "SharedTextureCapture.h"
#include "Vector2D.h"

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
		explicit CD3DCaptureSource(CD3DDevice& device);
		~CD3DCaptureSource();
		BOOL BeginCapture(const TinyString& processName);
		BOOL EndCapture();
		void Tick(FLOAT fSeconds);
		void Render(const Vector2D &pos, const Vector2D &size);
	private:
		BOOL	Initialize(const TinyString& processName);
		DWORD	FindProcess(const TinyString& processName);
		BOOL	FindWindow(const TinyString& processName);
		void	DrawSprite(DWORD color, float x, float y, float x2, float y2);
		SharedCapture* GetSharedCapture();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		CD3DDevice&					m_device;
		SharedCapture*				m_pSharedCapture;
		CD3D10Texture				m_gameTexture;//”Œœ∑µƒŒ∆¿Ì
		D3D::CSharedTextureCapture	m_textureCapture;
		TinyEvent					m_eventBegin;
		TinyEvent					m_eventEnd;
		TinyEvent					m_eventReady;
		TinyEvent					m_eventExit;
		TinySharedMemory			m_sharedCapture;
		WNDINFO						m_targetWND;
		BOOL						m_bCapturing;
	};
}

