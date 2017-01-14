#pragma once
#include "DX.h"

namespace DXCapture
{
	class DX10Capture
	{
	public:
		DX10Capture(DX& dx);
		~DX10Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX10GPUHook(ID3D10Device *device);
	public:
		BOOL SaveAs(D3DX10_IMAGE_FILE_FORMAT s, LPCSTR pzFile);
	public:
		SharedCaptureDATA				m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D10;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX10Capture g_dx10(g_dx);
}

