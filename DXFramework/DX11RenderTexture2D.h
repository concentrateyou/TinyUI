#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11RenderTexture2D
	{
	public:
		DX11RenderTexture2D(DX11& dx11);
		virtual ~DX11RenderTexture2D();
		DX11& GetDX11();
		TinySize GetSize() const;
		BOOL Create();
		BOOL Create(INT cx, INT cy);
		BOOL Resize();
		BOOL Resize(INT cx, INT cy);
		void BeginDraw();
		void EndDraw();
		ID3D11RenderTargetView* GetRTView() const;
		ID3D11DepthStencilView* GetDSView() const;
 	protected:
		DX11&								m_dx11;
		TinySize							m_size;
		TinyComPtr<ID3D11Texture2D>			m_depth2D;
		TinyComPtr<ID3D11DepthStencilView>	m_depthView;
		TinyComPtr<ID3D11Texture2D>			m_render2D;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
		
	};
}


