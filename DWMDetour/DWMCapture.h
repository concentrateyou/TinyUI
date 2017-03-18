#pragma once
#include "Common/TinyLogging.h"
#include "Common/TinyHook.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTaskBase.h"
#include "IDXGIFactoryDWM.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3d11.h>
using namespace TinyUI;

namespace DWM
{
#define DWM_WINDOWCLASS			TEXT("DWMCapture")

	typedef HRESULT(WINAPI *Present)(IDXGISwapChainDWM* swap, UINT sync_interval, UINT flags);
	typedef HRESULT(WINAPI *CreateDXGIFactory)(REFIID riid, _Out_ void   **ppFactory);
	typedef HRESULT(WINAPI *CreateSwapChain)(IDXGIFactoryDWM *factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChainDWM);

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
		HINSTANCE			m_hInstance;
	public:
		TinyDetour			m_createDXGIFactory;
		TinyDetour			m_createSwap;
	public:
		CreateDXGIFactory	m_origCreateDXGIFactory;
		CreateSwapChain		m_origCreateSwapChain;
	public:
		TinyComPtr<IDXGIFactoryDWM> m_dxgiFactoryDWM;
	};

	SELECTANY extern DWMCapture g_dwmCapture;
}


