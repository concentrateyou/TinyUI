#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX11 : public TinyLock
	{
		DISALLOW_COPY_AND_ASSIGN(DX11)
	public:
		DX11();
		~DX11();
		BOOL Initialize(HWND hWND, INT x, INT y, INT cx, INT cy);
		BOOL ResizeView(INT cx = 0, INT cy = 0);
		BOOL SetViewport(const TinyPoint& pos, const TinySize& size);
		void SetMatrixs(const TinySize& size);
		void BeginDraw(ID3D11RenderTargetView* pView = NULL);
		void EndDraw();
		ID3D11Device*			GetD3D() const;
		ID3D11DeviceContext*	GetImmediateContext() const;
		IDXGISwapChain*			GetSwap() const;
		ID3D11Texture2D*		GetTexture2D() const;
		ID3D11DepthStencilView* GetDSView() const;
		ID3D11RenderTargetView*	GetRTView() const;
		HWND					GetHWND() const;
		XMMATRIX				GetProjectionMatrix();
		XMMATRIX				GetWorldMatrix();
		XMMATRIX				GetOrthoMatrix();
		TinySize				GetSize() const;
	private:
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
		TinyComPtr<ID3D11Texture2D>			m_renderTexture;
		TinyComPtr<ID3D11DepthStencilView>	m_depthStencilView;
		TinyComPtr<ID3D11Texture2D>			m_depthStencilBuffer;
		TinyComPtr<ID3D11Device>			m_d3d;
		TinyComPtr<ID3D11DeviceContext>		m_immediateContext;
		TinyComPtr<ID3D11DepthStencilState>	m_depthStencilState;
		TinyComPtr<ID3D11RasterizerState>	m_rasterizerState;
		HWND								m_hWND;
		TinyPoint							m_pos;
		TinySize							m_size;
		XMMATRIX							m_projectionMatrix;
		XMMATRIX							m_worldMatrix;
		XMMATRIX							m_orthoMatrix;
	};
}

