#include "stdafx.h"
#include "DX11Texture.h"

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
	DX11Texture::DX11Texture()
	{
	}
	DX11Texture::~DX11Texture()
	{
	}
	BOOL DX11Texture::CreateTexture(const DX11& dx11, INT cx, INT cy, DXGI_FORMAT dxgi, void *lpData)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = cx;
		textureDesc.Height = cy;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = dxgi;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA dsd;
		D3D11_SUBRESOURCE_DATA *lpSRD = NULL;
		if (lpData)
		{
			dsd.pSysMem = lpData;
			dsd.SysMemPitch = cx * GetBits(dxgi);
			dsd.SysMemSlicePitch = 0;
			lpSRD = &dsd;
		}
		if (FAILED(dx11.GetD3D()->CreateTexture2D(&textureDesc, lpSRD, &m_texture2D)))
			return FALSE;
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Format = dxgi;
		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceDesc.Texture2D.MipLevels = 1;
		if (FAILED(dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &resourceDesc, &m_resourceView)))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture::FillTexture(const DX11& dx11, const BYTE* pBits)
	{
		ASSERT(m_texture2D);
		D3D11_MAPPED_SUBRESOURCE  mapResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
		if (FAILED(hRes))
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		INT linesize = ((((24 * desc.Width) + 31) / 32) * 4);
		BYTE* src = const_cast<BYTE*>(pBits);
		BYTE* dst = static_cast<BYTE*>(mapResource.pData);
		BOOL bFlipY = TRUE;
		for (INT row = 0; row < desc.Height; row++)
		{
			INT y = bFlipY ? desc.Height - 1 - row : row;
			for (INT col = 0; col < desc.Width; col++)
			{
				BYTE* iDst = src + y * linesize + col * 3;
				*dst++ = *iDst++;
				*dst++ = *iDst++;
				*dst++ = *iDst++;
				*dst++ = 0;
			}
		}
		dx11.GetImmediateContext()->Unmap(m_texture2D, 0);
		return TRUE;
	}
	BOOL DX11Texture::Save(const DX11& dx11, const CHAR* pzFile, D3DX11_IMAGE_FILE_FORMAT format)
	{
		ASSERT(m_texture2D);
		return D3DX11SaveTextureToFile(dx11.GetImmediateContext(), m_texture2D, format, pzFile) == S_OK;
	}
	BOOL DX11Texture::LoadTexture(const DX11& dx11, const CHAR* pzFile)
	{
		if (!pzFile)
			return FALSE;
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX11CreateShaderResourceViewFromFile(dx11.GetD3D(), pzFile, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D11Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		TinyComPtr<ID3D11Texture2D> texture2D;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2D)))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture::LoadTexture(const DX11& dx11, HANDLE hResource)
	{
		if (!hResource)
			return FALSE;
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
	BOOL DX11Texture::LoadTexture(const DX11& dx11, const BYTE* pBits, DWORD dwSize)
	{
		if (!pBits)
			return FALSE;
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX11CreateShaderResourceViewFromMemory(dx11.GetD3D(), pBits, dwSize, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D11Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_texture2D)))
			return FALSE;
		return TRUE;
	}
	ID3D11Texture2D* DX11Texture::GetTexture2D() const
	{
		return m_texture2D;
	}
	ID3D11ShaderResourceView* DX11Texture::GetSRView() const
	{
		return m_resourceView;
	}
	TinyComPtr<IDXGISurface1> DX11Texture::GetSurface()
	{
		TinyComPtr<IDXGISurface1> surface;
		m_texture2D->QueryInterface(__uuidof(IDXGISurface1), (void**)&surface);
		return surface;
	}
	TinySize DX11Texture::GetSize()
	{
		ASSERT(m_texture2D);
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		return TinySize(desc.Width, desc.Height);
	}
	BOOL DX11Texture::IsValid() const
	{
		return m_texture2D != NULL;
	}
}
