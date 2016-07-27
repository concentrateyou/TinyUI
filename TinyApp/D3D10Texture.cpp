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

	BOOL CD3D10Texture::CreateTexture(HANDLE hResource)
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
	BOOL CD3D10Texture::CreateTexture(UINT width, UINT height, DXGI_FORMAT format, void *lpData, BOOL bGenMipMaps, BOOL bStatic)
	{
		D3D10_TEXTURE2D_DESC dtd;
		::ZeroMemory(&dtd, sizeof(dtd));
		dtd.Width = width;
		dtd.Height = height;
		dtd.MipLevels = bGenMipMaps ? 0 : 1;
		dtd.ArraySize = 1;
		dtd.Format = format;
		dtd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		dtd.SampleDesc.Count = 1;
		dtd.Usage = bStatic ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
		dtd.CPUAccessFlags = bStatic ? 0 : D3D10_CPU_ACCESS_WRITE;
		D3D10_SUBRESOURCE_DATA dsd;
		D3D10_SUBRESOURCE_DATA *lpDSD = NULL;
		if (lpData)
		{
			// 0, 1, 1, 4, 4, 4, 4, 8, 16, 0, 0, 0
			dsd.pSysMem = lpData;
			//dsd.SysMemPitch = width * formatPitch[(UINT)colorFormat];
			dsd.SysMemSlicePitch = 0;
			lpDSD = &dsd;
		}
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(m_system->GetD3D()->CreateTexture2D(&dtd, lpDSD, &texture2D)))
		{
			return FALSE;
		}

		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = bGenMipMaps ? -1 : 1;
		ID3D10ShaderResourceView *resource;
		if (FAILED(m_system->GetD3D()->CreateShaderResourceView(texture2D, &dsrvd, &resource)))
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
