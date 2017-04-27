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
	BOOL DX11RenderTexture2D::Create(DX11& dx11)
	{
		TinyComPtr<ID3D11Texture2D> texture2D;
		HRESULT hRes = dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx11.GetD3D()->CreateRenderTargetView(texture2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = desc.Width;
		depthDesc.Height = desc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = dx11.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depthTexture2D);
		if (hRes != S_OK)
			return FALSE;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = dx11.GetD3D()->CreateDepthStencilView(m_depthTexture2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11RenderTexture2D::Create(DX11& dx11, INT cx, INT cy)
	{

		return TRUE;
	}
	ID3D11RenderTargetView* DX11RenderTexture2D::GetRTView() const
	{
		return m_renderView;
	}
	ID3D11DepthStencilView* DX11RenderTexture2D::GetDSView() const
	{
		return m_depthView;
	}
}
