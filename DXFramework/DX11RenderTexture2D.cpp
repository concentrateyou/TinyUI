#include "stdafx.h"
#include "DX11RenderTexture2D.h"

namespace DXFramework
{
	DX11RenderTexture2D::DX11RenderTexture2D(DX11& dx11)
		:m_dx11(dx11)
	{
	}

	DX11RenderTexture2D::~DX11RenderTexture2D()
	{
	}
	BOOL DX11RenderTexture2D::Create()
	{
		TinyComPtr<ID3D11Texture2D> texture2D;
		HRESULT hRes = m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx11.GetD3D()->CreateRenderTargetView(texture2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		m_size.cx = desc.Width;
		m_size.cy = desc.Height;
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
		hRes = m_dx11.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_dx11.GetD3D()->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11RenderTexture2D::Create(INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_size.cx;
		desc.Height = m_size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hRes = m_dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_render2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx11.GetD3D()->CreateRenderTargetView(m_render2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = m_size.cx;
		depthDesc.Height = m_size.cy;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_dx11.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_dx11.GetD3D()->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11RenderTexture2D::Resize()
	{
		m_renderView.Release();
		m_depth2D.Release();
		m_depthView.Release();
		return Create();
	}
	BOOL DX11RenderTexture2D::Resize(INT cx, INT cy)
	{
		m_renderView.Release();
		m_depth2D.Release();
		m_depthView.Release();
		if (m_render2D != NULL)
		{
			m_render2D.Release();
			return Create(cx, cy);
		}
		return FALSE;
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
