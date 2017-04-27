#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11RenderTexture2D
	{
	public:
		DX11RenderTexture2D();
		virtual ~DX11RenderTexture2D();
		BOOL Create(DX11& dx11);
		BOOL Create(DX11& dx11, INT cx, INT cy);
		ID3D11RenderTargetView* GetRTView() const;
		ID3D11DepthStencilView* GetDSView() const;
	protected:
		TinyComPtr<ID3D11Texture2D>			m_depthTexture2D;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
		TinyComPtr<ID3D11DepthStencilView>	m_depthView;
	};
}


