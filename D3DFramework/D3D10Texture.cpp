#include "stdafx.h"
#include "D3D10Texture.h"

namespace DX
{
	D3D10Texture::D3D10Texture()
	{
	}
	D3D10Texture::~D3D10Texture()
	{
	}
	BOOL D3D10Texture::Initialize(ID3D10Device* device, HANDLE hResource)
	{
		HRESULT hRes;
		TinyComPtr<ID3D10Resource> resource;
		if (FAILED(hRes = device->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&resource)))
			return FALSE;
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
			return FALSE;
		D3D10_TEXTURE2D_DESC dtd;
		texture2D->GetDesc(&dtd);
		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = dtd.Format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		if (FAILED(hRes = device->CreateShaderResourceView(texture2D, &dsrvd, &m_texture)))
			return FALSE;
		return TRUE;
	}
	ID3D10ShaderResourceView*  D3D10Texture::GetTexture() const
	{
		return m_texture;
	}
}

