#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX11RenderTexture2D;

	class DX11 : public TinyLock
	{
		DISALLOW_COPY_AND_ASSIGN(DX11)
	public:
		DX11();
		~DX11();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		BOOL ResizeView(INT cx = 0, INT cy = 0);
		void SetRenderTexture2D(DX11RenderTexture2D* render2D);
		void SetViewport(const TinyPoint& pos, const TinySize& size);
		void SetMatrixs(const TinySize& size);
		void Present();
		BOOL					IsValid() const;
		ID3D11Device*			GetD3D() const;
		ID3D11DeviceContext*	GetImmediateContext() const;
		IDXGISwapChain*			GetSwap() const;
		DX11RenderTexture2D*	GetRender2D() const;
		HWND					GetHWND() const;
		XMMATRIX*				GetMatrixs();
	private:
		TinyComPtr<ID3D11Device>			m_d3d;
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D11DeviceContext>		m_immediateContext;
		TinyComPtr<ID3D11DepthStencilState>	m_depthStencilState;
		TinyComPtr<ID3D11RasterizerState>	m_rasterizerState;
		TinyScopedPtr<DX11RenderTexture2D>	m_back2D;
		DX11RenderTexture2D*				m_render2D;
		HWND								m_hWND;
		XMMATRIX							m_matrixs[3];
	};
}

