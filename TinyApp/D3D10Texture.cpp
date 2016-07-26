#include "stdafx.h"
#include "D3D10Texture.h"

namespace D3D
{
	CD3D10Texture::CD3D10Texture(CD3DSystem* system)
		:m_system(system)
	{

	}


	CD3D10Texture::~CD3D10Texture()
	{
	}

	BOOL CD3D10Texture::GetTexture(HANDLE hResource)
	{
		ASSERT(m_system);
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Resource> d3d10Resource;
		if (FAILED(hRes = m_system->GetD3D()->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&d3d10Resource)))
		{
			return FALSE;
		}
		if (FAILED(hRes = d3d10Resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&m_d3d10Texture2D)))
		{
			return FALSE;
		}
		D3D10_TEXTURE2D_DESC dtd;
		m_d3d10Texture2D->GetDesc(&dtd);
		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = dtd.Format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		if (FAILED(hRes = m_system->GetD3D()->CreateShaderResourceView(m_d3d10Texture2D, &dsrvd, &m_d3d10SRView)))
		{
			return FALSE;
		}
		return TRUE;
	}
	TinyComPtr<IDXGISurface1> CD3D10Texture::GetSurface()
	{
		TinyComPtr<IDXGISurface1> surface;
		m_d3d10Texture2D->QueryInterface(__uuidof(IDXGISurface1), (void**)&surface);
		return surface;
	}
}
