#include "stdafx.h"
#include "D3D10Texture.h"

namespace DXFramework
{
	D3D10Texture::D3D10Texture()
	{
	}
	D3D10Texture::~D3D10Texture()
	{
	}
	BOOL D3D10Texture::Initialize(ID3D10Device* device, HANDLE hResource)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Resource> resource;
		if (FAILED(hRes = device->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&resource)))
			return FALSE;
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
			return FALSE;
		D3D10_TEXTURE2D_DESC dtd;
		texture2D->GetDesc(&dtd);
		m_size.cx = dtd.Width;
		m_size.cy = dtd.Height;
		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = dtd.Format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		if (FAILED(hRes = device->CreateShaderResourceView(texture2D, &dsrvd, &m_texture)))
			return FALSE;
		return TRUE;
	}
	BOOL D3D10Texture::Initialize(ID3D10Device* device, CHAR* pzFile)
	{
		if (D3DX10CreateShaderResourceViewFromFile(device, pzFile, NULL, NULL, &m_texture, NULL) == S_OK)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<ID3D10Resource> resource;
			m_texture->GetResource(&resource);
			TinyComPtr<ID3D10Texture2D> texture2D;
			if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
				return FALSE;
			D3D10_TEXTURE2D_DESC dtd;
			texture2D->GetDesc(&dtd);
			m_size.cx = dtd.Width;
			m_size.cy = dtd.Height;
			return TRUE;
		}
		return FALSE;
	}
	ID3D10ShaderResourceView*  D3D10Texture::GetTexture() const
	{
		return m_texture;
	}
}

