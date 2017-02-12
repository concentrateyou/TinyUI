#pragma once
#include "DX.h"


namespace DXCapture
{
	class DX101Capture
	{
	public:
		DX101Capture(DX& dx);
		~DX101Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
	//public:
	//	BOOL SaveAs(D3DX10_IMAGE_FILE_FORMAT s, LPCSTR pzFile);
	public:
		SharedCaptureDATA				m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D101;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX101Capture g_dx101(g_dx);
}

