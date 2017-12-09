#pragma once
#include "Common/TinyLogging.h"
#include "Common/TinyHook.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTask.h"
#include "IDXGIFactoryDWM.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3d11.h>
using namespace TinyUI;

namespace DWM
{
#define DWM_WINDOWCLASS			TEXT("DWM WindowClass")

	typedef struct tagDWMCaptureDATA
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		BOOL		bMultisample;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}DWMCaptureDATA;

	class DWMCapture
	{
	public:
		DWMCapture();
		~DWMCapture();
		BOOL Attach(HMODULE hModule);
		BOOL Detach(HMODULE hModule);
	public:
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		void Release();
	public:
		BOOL BeginCapture();
		BOOL EndCapture();
		void OnMessagePump();
		BOOL DX101GPUHook(ID3D10Device1 *device);
	private:
		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		HWND				m_hWNDD3D;
		IO::TinyTask	m_task;
		HINSTANCE			m_hInstance;
	public:
		DWMCaptureDATA					m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		BOOL							m_bDX101;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D101;
		IDXGISwapChain*					m_currentSwap;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
	};

	SELECTANY extern DWMCapture g_dwm;
}


