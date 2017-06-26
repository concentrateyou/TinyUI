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
		:m_handle(FALSE)
	{
	}

	DX11Texture2D::~DX11Texture2D()
	{
	}

	BOOL DX11Texture2D::Create(DX11& dx11, INT cx, INT cy, BOOL bShared)
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
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = bShared ? D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_GDI_COMPATIBLE : D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&textureDesc, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		m_texture2D->GetDesc(&desc);
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		::ZeroMemory(&dsrvd, sizeof(dsrvd));
		dsrvd.Format = desc.Format;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsrvd.Texture2D.MipLevels = 1;
		hRes = dx11.GetD3D()->CreateShaderResourceView(m_texture2D, &dsrvd, &m_resourceView);
		if (hRes != S_OK)
			return FALSE;
		if (bShared)
		{
			TinyComPtr<IDXGIResource> resource;
			if (FAILED(hRes = m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
				return FALSE;
			if (FAILED(hRes = resource->GetSharedHandle(&m_handle)))
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
		if (!m_surface)
			return FALSE;
		HRESULT hRes = m_surface->ReleaseDC(NULL);
		m_surface.Release();
		return SUCCEEDED(hRes);
	}
	BOOL DX11Texture2D::Map(DX11& dx11, BYTE *&lpData, UINT &pitch)
	{
		HRESULT hRes = S_OK;
		D3D11_MAPPED_SUBRESOURCE ms = { 0 };
		if (SUCCEEDED(dx11.GetImmediateContext()->Map(m_texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
		{
			lpData = (BYTE*)ms.pData;
			pitch = ms.RowPitch;
			return TRUE;
		}
		return FALSE;
	}
	void DX11Texture2D::Unmap(DX11& dx11)
	{
		dx11.GetImmediateContext()->Unmap(m_texture2D, 0);
	}
	BOOL DX11Texture2D::Create(DX11& dx11, ID3D11Texture2D* texture2D)
	{
		if (!texture2D)
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		HRESULT hRes = dx11.GetD3D()->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
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
	BOOL DX11Texture2D::Create(DX11& dx11, INT cx, INT cy, const BYTE* bits, BOOL bReadoly)
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
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = bReadoly ? 0 : D3D11_CPU_ACCESS_WRITE;
		textureDesc.Usage = bReadoly ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
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
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC desc;
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
	BOOL DX11Texture2D::SaveAs(DX11& dx11, const CHAR* pzFile, D3DX11_IMAGE_FILE_FORMAT format)
	{
		ASSERT(m_texture2D);
		wstring ws = StringToWString(pzFile);
		HRESULT hRes = SaveWICTextureToFile(dx11.GetImmediateContext(), m_texture2D, GetWICCodec(format), ws.c_str());
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
	BOOL DX11Texture2D::Load(DX11& dx11, const CHAR* pzFile)
	{
		ASSERT(pzFile);
		m_texture2D.Release();
		m_resourceView.Release();
		HRESULT hRes = S_OK;
		wstring wpzFile = StringToWString(pzFile);
		TinyComPtr<ID3D11Resource> resource;
		hRes = CreateWICTextureFromFile(dx11.GetD3D(), wpzFile.c_str(), &resource, &m_resourceView);
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
		TinyComPtr<ID3D11Resource> resource;
		if (!CreateWICTextureFromMemory(dx11.GetD3D(), bits, dwSize, &resource, &m_resourceView))
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

	HANDLE DX11Texture2D::GetHandle() const
	{
		return m_handle;
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
