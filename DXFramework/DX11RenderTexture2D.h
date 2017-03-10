#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11RenderTexture2D : public DX11Texture2D
	{
	public:
		DX11RenderTexture2D();
		virtual ~DX11RenderTexture2D();
		virtual BOOL Create(DX11& dx11, INT cx, INT cy);
		ID3D11RenderTargetView* GetTRView() const;
	public:
		void Destory() OVERRIDE;
	protected:
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
	};
}


