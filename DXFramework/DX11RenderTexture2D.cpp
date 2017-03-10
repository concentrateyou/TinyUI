#include "stdafx.h"
#include "DX11RenderTexture2D.h"

namespace DXFramework
{
	DX11RenderTexture2D::DX11RenderTexture2D()
	{
	}

	DX11RenderTexture2D::~DX11RenderTexture2D()
	{
	}
	BOOL DX11RenderTexture2D::Create(DX11& dx11, INT cx, INT cy)
	{
		m_texture2D.Release();
		m_renderView.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = desc.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hRes = dx11.GetD3D()->CreateRenderTargetView(m_texture2D, &rtvd, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX11RenderTexture2D::Destory()
	{
		m_renderView.Release();
		DX11Texture2D::Destory();
	}
	ID3D11RenderTargetView* DX11RenderTexture2D::GetTRView() const
	{
		return m_renderView;
	}
}
