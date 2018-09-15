#include "stdafx.h"
#include "DX11Texture2D.h"

namespace DXFramework
{
	DX11Texture2D::DX11Texture2D()
	{
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	DX11Texture2D::operator ID3D11Texture2D*() const
	{
		return m_texture2D;
	}
	BOOL DX11Texture2D::Create(DX11& dx11, INT cx, INT cy)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		desc.Usage = D3D11_USAGE_DEFAULT;//GPU快速读写
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		ZeroMemory(&desc, sizeof(desc));
		m_texture2D->GetDesc(&desc);
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture2D::Create(DX11& dx11, D3D11_TEXTURE2D_DESC& desc)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
			::ZeroMemory(&dsrvd, sizeof(dsrvd));
			dsrvd.Format = desc.Format;
			dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			dsrvd.Texture2D.MipLevels = 1;
			hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
			if (hRes != S_OK)
				return FALSE;
		}
		return TRUE;
	}
	BOOL DX11Texture2D::Create(DX11& dx11, ID3D11Texture2D* texture2D)
	{
		if (!texture2D)
			return FALSE;
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
			::ZeroMemory(&dsrvd, sizeof(dsrvd));
			dsrvd.Format = desc.Format;
			dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			dsrvd.Texture2D.MipLevels = 1;
			hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
			if (hRes != S_OK)
				return FALSE;
		}
		return TRUE;
	}
	BOOL DX11Texture2D::Create(DX11& dx11, INT cx, INT cy, const BYTE* bits, BOOL bReadoly)
	{
		m_texture2D.Release();
		m_resourceView.Release();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = bReadoly ? 0 : D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = bReadoly ? D3D11_CPU_ACCESS_READ : D3D11_CPU_ACCESS_WRITE;
		//D3D11_USAGE_DYNAMIC 快速的CPU写和较慢的GPU读 Map/Unmap
		//D3D11_USAGE_STAGING 读取GPU数据 Map/Unmap
		desc.Usage = bReadoly ? D3D11_USAGE_STAGING : D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA dsd;
		D3D11_SUBRESOURCE_DATA *lpSRD = NULL;
		if (bits)
		{
			dsd.pSysMem = static_cast<void*>(const_cast<BYTE*>(bits));
			dsd.SysMemPitch = cx * 4;
			dsd.SysMemSlicePitch = 0;
			lpSRD = &dsd;
		}
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, lpSRD, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
			::ZeroMemory(&dsrvd, sizeof(dsrvd));
			dsrvd.Format = desc.Format;
			dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			dsrvd.Texture2D.MipLevels = desc.MipLevels;
			hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
			if (hRes != S_OK)
				return FALSE;
		}
		return TRUE;
	}
	BOOL  DX11Texture2D::GetDC(BOOL discard, HDC& hDC)
	{
		if (!m_texture2D)
			return FALSE;
		m_surface.Release();
		HRESULT hRes = m_texture2D->QueryInterface(__uuidof(IDXGISurface1), (void**)&m_surface);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_surface->GetDC(discard, &hDC);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11Texture2D::ReleaseDC()
	{
		if (m_surface != NULL)
		{
			HRESULT hRes = m_surface->ReleaseDC(NULL);
			m_surface.Release();
			return SUCCEEDED(hRes);
		}
		return FALSE;
	}
	BOOL DX11Texture2D::Map(DX11& dx11, D3D11_MAPPED_SUBRESOURCE& ms, BOOL bReadoly)
	{
		if (m_texture2D != NULL)
		{
			HRESULT hRes = dx11.GetImmediateContext()->Map(m_texture2D, 0, bReadoly ? D3D11_MAP_READ : D3D11_MAP_WRITE_DISCARD, 0, &ms);
			return SUCCEEDED(hRes);
		}
		return FALSE;
	}
	void DX11Texture2D::Unmap(DX11& dx11)
	{
		if (m_texture2D != NULL)
		{
			dx11.GetImmediateContext()->Unmap(m_texture2D, 0);
		}
	}
	BOOL DX11Texture2D::SaveAs(DX11& dx11, const CHAR* pzFile, IMAGE_FILE_FORMAT format)
	{
		ASSERT(m_texture2D);
		wstring ws = StringToWString(pzFile);
		HRESULT hRes = DX11SaveWICTextureToFile(dx11.GetImmediateContext(), m_texture2D, GetWICCodec(format), ws.c_str(), &GUID_WICPixelFormat32bppBGRA, NULL);
		if (SUCCEEDED(hRes))
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Texture2D::Copy(DX11& dx11, ID3D11Texture2D* texture2D)
	{
		if (!m_texture2D || !texture2D)
			return FALSE;
		dx11.GetImmediateContext()->CopyResource(m_texture2D, texture2D);
		return TRUE;
	}
	BOOL DX11Texture2D::Copy(DX11& dx11, DX11Texture2D& texture2D)
	{
		if (!m_texture2D || texture2D.IsEmpty())
			return FALSE;
		dx11.GetImmediateContext()->CopyResource(m_texture2D, texture2D.GetTexture2D());
		return TRUE;
	}
	BOOL DX11Texture2D::Copy(DX11& dx11, D3D11_BOX* ps, const BYTE* bits, LONG size, UINT rowPitch, UINT depthPitch)
	{
		if (!m_texture2D || !bits || size <= 0)
			return FALSE;
		dx11.GetImmediateContext()->UpdateSubresource(m_texture2D, 0, ps, static_cast<const void*>(bits), rowPitch, depthPitch);
		return TRUE;
	}
	BOOL DX11Texture2D::Load(DX11& dx11, const CHAR* pzFile)
	{
		ASSERT(pzFile);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		wstring wpzFile = StringToWString(pzFile);
		TinyComPtr<ID3D11Resource> resource;
		hRes = DX11CreateWICTextureFromFile(dx11.GetD3D(), wpzFile.c_str(), &resource, &m_resourceView);
		if (hRes != S_OK)
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
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
			::ZeroMemory(&dsrvd, sizeof(dsrvd));
			dsrvd.Format = desc.Format;
			dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			dsrvd.Texture2D.MipLevels = desc.MipLevels;
			if (FAILED(hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView)))
				return FALSE;
		}
		return TRUE;
	}
	BOOL DX11Texture2D::Load(DX11& dx11, const BYTE* bits, LONG size)
	{
		ASSERT(bits);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11Resource> resource;
		if (!DX11CreateWICTextureFromMemory(dx11.GetD3D(), bits, size, &resource, &m_resourceView))
			return FALSE;
		if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_texture2D)))
			return FALSE;
		return TRUE;
	}
	void DX11Texture2D::Destory()
	{
		m_surface.Release();
		m_resourceView.Release();
		m_texture2D.Release();
	}
	HANDLE DX11Texture2D::GetHandle()
	{
		HANDLE handle = NULL;
		do
		{
			if (!m_texture2D)
				break;
			TinyComPtr<IDXGIResource> resource;
			if (FAILED(m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
				break;
			if (FAILED(resource->GetSharedHandle(&handle)))
				break;
		} while (0);
		return handle;
	}
	ID3D11Texture2D* DX11Texture2D::GetTexture2D() const
	{
		return m_texture2D;
	}
	ID3D11ShaderResourceView* DX11Texture2D::GetSRView() const
	{
		return m_resourceView;
	}
	BOOL DX11Texture2D::IsEmpty() const
	{
		return (m_texture2D == NULL);
	}
}
