#include "stdafx.h"
#include "D3D10Texture.h"

namespace D3D
{
	inline GSColorFormat ConvertGIBackBufferFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return GS_RGBA;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return GS_BGRA;
		case DXGI_FORMAT_R10G10B10A2_UNORM: return GS_R10G10B10A2;
		case DXGI_FORMAT_B8G8R8X8_UNORM:    return GS_BGR;
		case DXGI_FORMAT_B5G5R5A1_UNORM:    return GS_B5G5R5A1;
		case DXGI_FORMAT_B5G6R5_UNORM:      return GS_B5G6R5;
		}

		return GS_UNKNOWNFORMAT;
	}
	//////////////////////////////////////////////////////////////////////////
	CD3D10Texture::CD3D10Texture(CD3DDevice& system)
		:m_device(system)
	{

	}


	CD3D10Texture::~CD3D10Texture()
	{

	}
	BOOL CD3D10Texture::IsValid() const
	{
		return m_d3d10Texture2D && m_d3d10SRView && m_d3dRenderTarget;
	}
	CD3DDevice* CD3D10Texture::GetSystem()
	{
		return &m_device;
	}
	TinySize CD3D10Texture::GetSize() const
	{
		return m_size;
	}
	BOOL CD3D10Texture::CreateTexture(HANDLE hResource)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Resource> d3d10Resource;
		if (FAILED(hRes = m_device.GetD3D()->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&d3d10Resource)))
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
		if (FAILED(hRes = m_device.GetD3D()->CreateShaderResourceView(m_d3d10Texture2D, &dsrvd, &m_d3d10SRView)))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL CD3D10Texture::CreateTexture(const SIZE& size, DXGI_FORMAT format, void *lpData, BOOL bGenMipMaps, BOOL bStatic)
	{
		D3D10_TEXTURE2D_DESC dtd;
		::ZeroMemory(&dtd, sizeof(dtd));
		dtd.Width = size.cx;
		dtd.Height = size.cy;
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
			dsd.pSysMem = lpData;
			GSColorFormat gcf = ConvertGIBackBufferFormat(format);
			dsd.SysMemPitch = size.cx * formatPitch[(UINT)gcf];
			dsd.SysMemSlicePitch = 0;
			lpDSD = &dsd;
		}
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(m_device.GetD3D()->CreateTexture2D(&dtd, lpDSD, &texture2D)))
		{
			return FALSE;
		}

		D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = format;
		dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = bGenMipMaps ? -1 : 1;
		ID3D10ShaderResourceView *resource;
		if (FAILED(m_device.GetD3D()->CreateShaderResourceView(texture2D, &dsrvd, &resource)))
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
