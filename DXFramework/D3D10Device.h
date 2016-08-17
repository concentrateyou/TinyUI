#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class D3D10Device
	{
		DISALLOW_COPY_AND_ASSIGN(D3D10Device)
	public:
		D3D10Device();
		~D3D10Device();
		BOOL Initialize(HWND hWND, INT cx, INT cy, FLOAT nearZ, FLOAT farZ);
		//TRUE »æÖÆ3D FALSE »æÖÆ2D
		void AllowDepth(BOOL bEnable);
		void BeginScene(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		void EndScene();
		ID3D10Device1* GetDevice() const;
	private:
		TinyComPtr<IDXGISwapChain>				m_swapChain;
		TinyComPtr<ID3D10Device1>				m_device;
		TinyComPtr<ID3D10Texture2D>				m_depthStencilBuffer;
		TinyComPtr<ID3D10RenderTargetView>		m_renderTargetView;
		TinyComPtr<ID3D10RasterizerState>		m_rasterState;
		TinyComPtr<ID3D10DepthStencilView>		m_depthStencilView;
		TinyComPtr<ID3D10DepthStencilState>		m_depthStencilState;
		TinyComPtr<ID3D10DepthStencilState>		m_disableDepthStencilState;
		D3DXMATRIX								m_projectionMatrix;
		D3DXMATRIX								m_worldMatrix;
		D3DXMATRIX								m_orthoMatrix;
		FLOAT									m_fovY;
		FLOAT									m_aspect;
		FLOAT									m_nearZ;
		FLOAT									m_farZ;
		INT										m_cx;
		INT										m_cy;
	};
}



