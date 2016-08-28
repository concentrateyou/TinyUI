#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX11
	{
		DISALLOW_COPY_AND_ASSIGN(DX11)
	public:
		DX11();
		~DX11();
		BOOL Initialize(HWND hWND, INT x, INT y, INT cx, INT cy);
		BOOL ResizeView();
		void BeginScene();
		void EndScene();
		void AllowDepth(BOOL allow);
		ID3D11Device*			GetD3D() const;
		ID3D11DeviceContext*	GetContext() const;
		HWND					GetHWND() const;
		D3DXMATRIX				GetProjectionMatrix();
		D3DXMATRIX				GetWorldMatrix();
		D3DXMATRIX				GetOrthoMatrix();
		TinySize				GetSize() const;
	private:
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
		TinyComPtr<ID3D11DepthStencilView>	m_depthStencilView;
		TinyComPtr<ID3D11Texture2D>			m_depthStencilBuffer;
		TinyComPtr<ID3D11Device>			m_d3d;
		TinyComPtr<ID3D11DeviceContext>		m_context;
		TinyComPtr<ID3D11DepthStencilState>	m_depthStencilState;
		TinyComPtr<ID3D11DepthStencilState> m_disableDepthState;
		TinyComPtr<ID3D11RasterizerState>	m_rasterizerState;
		HWND								m_hWND;
		POINT								m_pos;
		TinySize							m_size;
		D3DXMATRIX							m_projectionMatrix;
		D3DXMATRIX							m_worldMatrix;
		D3DXMATRIX							m_orthoMatrix;
	};
}

