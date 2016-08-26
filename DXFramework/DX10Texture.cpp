#include "stdafx.h"
#include "DX10Texture.h"

namespace DXFramework
{
	DX10Texture::DX10Texture()
	{
	}


	DX10Texture::~DX10Texture()
	{
	}
	BOOL DX10Texture::CreateTexture(const DX10& dx10, INT cx, INT cy)
	{
		TinyScopedArray<BYTE> bits(new BYTE[cx*cy * 4]);
		memset(bits.Ptr(), 0xFFFF0000, cx*cy * 4);
		D3D10_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = cx;
		textureDesc.Height = cy;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D10_USAGE_DYNAMIC;
		textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		D3D10_SUBRESOURCE_DATA srd;
		D3D10_SUBRESOURCE_DATA *lpSRD = NULL;
		if (bits.Ptr())
		{
			srd.pSysMem = static_cast<const void*>(bits.Ptr());
			srd.SysMemPitch = cx * 4;
			srd.SysMemSlicePitch = 0;
			lpSRD = &srd;
		}
		if (FAILED(dx10.GetD3D()->CreateTexture2D(&textureDesc, lpSRD, &m_texture2D)))
			return FALSE;
		D3D10_SHADER_RESOURCE_VIEW_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		resourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		resourceDesc.Texture2D.MipLevels = 1;
		if (FAILED(dx10.GetD3D()->CreateShaderResourceView(m_texture2D, &resourceDesc, &m_resourceView)))
			return FALSE;
		return TRUE;
	}
	BOOL DX10Texture::FillTexture(const BYTE* pBits, INT cx, INT cy)
	{
		ASSERT(m_texture2D);
		D3D10_MAPPED_TEXTURE2D map;
		HRESULT hRes = m_texture2D->Map(0, D3D10_MAP_WRITE_DISCARD, 0, &map);
		if (FAILED(hRes))
			return FALSE;
		INT linesize = ((((24 * cx) + 31) / 32) * 4);
		BYTE* src = const_cast<BYTE*>(pBits);
		BYTE* dst = static_cast<BYTE*>(map.pData);
		BOOL bFlipY = TRUE;
		for (INT row = 0; row < cy; row++)
		{
			INT y = bFlipY ? cy - 1 - row : row;
			for (INT col = 0; col < cx; col++)
			{
				BYTE* iDst = src + y * linesize + col * 3;
				*dst++ = *iDst++;
				*dst++ = *iDst++;
				*dst++ = *iDst++;
				*dst++ = 0;
			}
		}
		m_texture2D->Unmap(0);
		return TRUE;
	}
	BOOL DX10Texture::Save(const CHAR* pzFile, D3DX10_IMAGE_FILE_FORMAT format)
	{
		ASSERT(m_texture2D);
		return D3DX10SaveTextureToFile(m_texture2D, format, pzFile) == S_OK;
	}
	BOOL DX10Texture::LoadTexture(const DX10& dx10, const CHAR* pzFile)
	{
		if (!pzFile)
			return FALSE;
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX10CreateShaderResourceViewFromFile(dx10.GetD3D(), pzFile, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D10Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
			return FALSE;
		return TRUE;
	}
	BOOL DX10Texture::LoadTexture(const DX10& dx10, HANDLE hResource)
	{
		if (!hResource)
			return FALSE;
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Resource> resource;
		if (FAILED(hRes = dx10.GetD3D()->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&m_texture2D)))
			return FALSE;
		D3D10_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		if (FAILED(hRes = dx10.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView)))
			return FALSE;
		return TRUE;
	}
	BOOL DX10Texture::LoadTexture(const DX10& dx10, const BYTE* pBits, INT size)
	{
		if (!pBits)
			return FALSE;
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX10CreateShaderResourceViewFromMemory(dx10.GetD3D(), pBits, size, NULL, NULL, &m_resourceView, NULL)))
			return FALSE;
		TinyComPtr<ID3D10Resource> resource;
		m_resourceView->GetResource(&resource);
		if (!resource)
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&m_texture2D)))
			return FALSE;
		return TRUE;
	}
	ID3D10Texture2D* DX10Texture::GetTexture2D() const
	{
		return m_texture2D;
	}
	ID3D10ShaderResourceView* DX10Texture::GetSRView() const
	{
		return m_resourceView;
	}
	TinySize DX10Texture::GetSize()
	{
		ASSERT(m_texture2D);
		D3D10_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		return TinySize(desc.Width, desc.Height);
	}
	BOOL DX10Texture::IsValid() const
	{
		return m_texture2D != NULL;
	}
}
