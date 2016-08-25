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

	BOOL DX10Texture::LoadTexture(const DX10& dx10, const CHAR* pzFile)
	{
		if (!pzFile)
			return FALSE;
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX10CreateShaderResourceViewFromFile(dx10.GetD3D(), pzFile, NULL, NULL, &m_view, NULL)))
			return FALSE;
		TinyComPtr<ID3D10Resource> resource;
		m_view->GetResource(&resource);
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
		if (FAILED(hRes = dx10.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_view)))
			return FALSE;
		return TRUE;
	}

	BOOL DX10Texture::LoadTexture(const DX10& dx10, const BYTE* data, INT size)
	{
		if (!data)
			return FALSE;
		HRESULT hRes = S_OK;
		if (FAILED(hRes = D3DX10CreateShaderResourceViewFromMemory(dx10.GetD3D(), data, size, NULL, NULL, &m_view, NULL)))
			return FALSE;
		TinyComPtr<ID3D10Resource> resource;
		m_view->GetResource(&resource);
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
		return m_view;
	}
	SIZE DX10Texture::GetSize()
	{
		ASSERT(m_texture2D);
		D3D10_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		return{ desc.Width, desc.Height };
	}
}
