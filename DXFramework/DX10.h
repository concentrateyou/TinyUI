#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX10
	{
		DISALLOW_COPY_AND_ASSIGN(DX10)
	public:
		DX10();
		~DX10();
		BOOL Initialize(HWND hWND, INT x, INT y, INT cx, INT cy);
		BOOL ResizeView();
		void BeginScene();
		void EndScene();
		void AllowDepth(BOOL allow);
		ID3D10Device1*	GetD3D() const;
		HWND			GetHWND() const;
		D3DXMATRIX		GetProjectionMatrix();
		D3DXMATRIX		GetWorldMatrix();
		D3DXMATRIX		GetOrthoMatrix();
		SIZE			GetSize() const;
	private:
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<IDXGIFactory1>			m_factory;
		TinyComPtr<ID3D10RenderTargetView>	m_renderView;
		TinyComPtr<ID3D10DepthStencilView>	m_depthStencilView;
		TinyComPtr<ID3D10Device1>			m_d3d;
		TinyComPtr<ID3D10DepthStencilState>	m_depthStencilState;
		TinyComPtr<ID3D10DepthStencilState> m_disableDepthState;
		TinyComPtr<ID3D10RasterizerState>	m_rasterizerState;
		TinyComPtr<ID3D10RasterizerState>	m_scissorState;
		TinyComPtr<ID3D10BlendState>		m_blendState;
		TinyComPtr<ID3D10BlendState>		m_disableblendState;
		HWND								m_hWND;
		POINT								m_pos;
		SIZE								m_size;
		D3DXMATRIX							m_projectionMatrix;
		D3DXMATRIX							m_worldMatrix;
		D3DXMATRIX							m_orthoMatrix;
	};
}

