#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX101GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX101GraphicsCapture)
	public:
		DX101GraphicsCapture(DX& dx);
		~DX101GraphicsCapture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
	public:
		DX&								m_dx;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		HANDLE							m_handle;
		HMODULE							m_hD3D101;
		DXGI_FORMAT						m_dxgiFormat;
		HookDATA						m_hookDATA;
		TinyComPtr<ID3D10Texture2D>		m_copy2D;
		TinyComPtr<ID3D10Texture2D>		m_texture2D;
	};
	SELECTANY extern DX101GraphicsCapture g_dx101(g_dx);
}

