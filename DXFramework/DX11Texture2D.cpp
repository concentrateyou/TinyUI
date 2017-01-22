#include "stdafx.h"
#include "DX11Texture2D.h"

namespace DXFramework
{
	INT GetBits(DXGI_FORMAT dxgi)
	{
		switch (dxgi)
		{
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return 4;
		default:
			return 2;
		}
	}
	DX11Texture2D::DX11Texture2D()
		:m_bCompatible(FALSE)
	{
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}

	BOOL DX11Texture2D::CreateCompatible(DX11& dx11, INT cx, INT cy, const BYTE* bits)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = cx;
		textureDesc.Height = cy;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA dsd;
		D3D11_SUBRESOURCE_DATA *lpSRD = NULL;
		if (bits)
		{
			dsd.pSysMem = static_cast<void*>(const_cast<BYTE*>(bits));
			dsd.SysMemPitch = cx * 4;
			dsd.SysMemSlicePitch = 0;
			lpSRD = &dsd;
		}
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&textureDesc, lpSRD, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
		if (FAILED(hRes))
			return FALSE;
		m_bCompatible = TRUE;
		return TRUE;
	}
	BOOL  DX11Texture2D::GetDC(HDC& hDC)
	{
		if (!m_texture2D || !m_bCompatible)
			return FALSE;
		HRESULT hRes = m_texture2D->QueryInterface(__uuidof(IDXGISurface1), (void**)&m_surface);
		if (FAILED(hRes))
			return FALSE;
		return m_surface->GetDC(FALSE, &hDC) == S_OK;
	}
	BOOL DX11Texture2D::ReleaseDC()
	{
		if (!m_surface || !m_bCompatible)
			return FALSE;
		m_surface->ReleaseDC(NULL);
		m_surface.Release();
		return TRUE;
	}
	BOOL DX11Texture2D::Create(DX11& dx11, INT cx, INT cy, const BYTE* bits)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = cx;
		textureDesc.Height = cy;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA dsd;
		D3D11_SUBRESOURCE_DATA *lpSRD = NULL;
		if (bits)
		{
			dsd.pSysMem = static_cast<void*>(const_cast<BYTE*>(bits));
			dsd.SysMemPitch = cx * 4;
			dsd.SysMemSlicePitch = 0;
			lpSRD = &dsd;
		}
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&textureDesc, lpSRD, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture2D::Save(DX11& dx11, const CHAR* pzFile, D3DX11_IMAGE_FILE_FORMAT format)
	{
		ASSERT(m_texture2D);
		return D3DX11SaveTextureToFile(dx11.GetImmediateContext(), m_texture2D, format, pzFile) == S_OK;
	}
	BOOL DX11Texture2D::Load(DX11& dx11, const CHAR* pzFile)
	{
		ASSERT(pzFile);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX11CreateShaderResourceViewFromFile(dx11.GetD3D(), pzFile, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D11Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_texture2D)))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture2D::Load(DX11& dx11, HANDLE hResource)
	{
		ASSERT(hResource);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11Resource> resource;
		if (FAILED(hRes = dx11.GetD3D()->OpenSharedResource(hResource, __uuidof(ID3D11Resource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_texture2D)))
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		if (FAILED(hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView)))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture2D::Load(DX11& dx11, const BYTE* bits, DWORD dwSize)
	{
		ASSERT(bits);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX11CreateShaderResourceViewFromMemory(dx11.GetD3D(), bits, dwSize, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D11Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_texture2D)))
			return FALSE;
		return TRUE;
	}
	void DX11Texture2D::Destory()
	{
		m_bCompatible = FALSE;
		m_surface.Release();
		m_resourceView.Release();
		m_texture2D.Release();
	}
	BOOL DX11Texture2D::IsCompatible() const
	{
		return m_bCompatible;
	}
	ID3D11Texture2D* DX11Texture2D::GetTexture2D() const
	{
		return m_texture2D;
	}
	ID3D11ShaderResourceView* DX11Texture2D::GetSRView() const
	{
		return m_resourceView;
	}
	TinySize DX11Texture2D::GetSize()
	{
		ASSERT(m_texture2D);
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		return TinySize(desc.Width, desc.Height);
	}
	BOOL DX11Texture2D::IsEmpty() const
	{
		return m_texture2D != NULL;
	}
}
