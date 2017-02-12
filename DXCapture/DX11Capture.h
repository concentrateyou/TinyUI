#pragma once
#include "DX.h"

namespace DXCapture
{
	class DX11Capture
	{
	public:
		DX11Capture(DX& dx);
		~DX11Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *pThis, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *pThis);
	//public:
	//	BOOL SaveAs(ID3D11DeviceContext* context, D3DX11_IMAGE_FILE_FORMAT s, LPCSTR pzFile);
	public:
		SharedCaptureDATA				m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D11;
		TinyComPtr<ID3D11Resource>		m_resource;
		DX&								m_dx;
	};
	SELECTANY extern DX11Capture g_dx11(g_dx);
}

