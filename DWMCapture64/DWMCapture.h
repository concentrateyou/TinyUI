#pragma once
#include "Common/TinyCommon.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTaskBase.h"
using namespace TinyUI;

namespace DWM
{
#define DWM_WINDOWCLASS			TEXT("DWMCapture")

	typedef HRESULT(WINAPI *CreateDXGIFactory)(
		REFIID riid,
		_Out_ void   **ppFactory
		);

	class DWMCapture
	{
	public:
		DWMCapture();
		~DWMCapture();
		BOOL Attach(HMODULE hModule);
		BOOL Detach(HMODULE hModule);
	private:
		BOOL BeginCapture();
		BOOL EndCapture();
		void OnMessagePump();
	private:
		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		HWND				m_hWNDD3D;
		IO::TinyTaskBase	m_task;
		TinyDetour			m_createDXGIFactory;
		TinyDetour			m_createSwap;
		HINSTANCE			m_hInstance;
	public:
		TinyComPtr<IDXGIFactoryDWM> m_dxgiFactoryDWM;
	};

	SELECTANY extern DWMCapture g_dwmCapture;
}


