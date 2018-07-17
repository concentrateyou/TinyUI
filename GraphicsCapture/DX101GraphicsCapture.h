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
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
	public:
		DX&								m_dx;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D101;
		DXGI_FORMAT						m_dxgiFormat;
		SharedCaptureDATA				m_captureDATA;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyComPtr<ID3D10Texture2D>		m_copy2D;
	};
	SELECTANY extern DX101GraphicsCapture g_dx101(g_dx);
}

