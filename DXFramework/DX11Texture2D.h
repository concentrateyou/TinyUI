#pragma once
#include "DXFramework.h"
#include "DX11.h"
#include "WICTexture.h"

namespace DXFramework
{
	/// <summary>
	/// 2D纹理
	/// </summary>
	class DX11Texture2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Texture2D)
	public:
		DX11Texture2D();
		DX11Texture2D(ID3D11Texture2D* texture2D);
		virtual ~DX11Texture2D();
	public:
		virtual BOOL Create(DX11& dx11, D3D11_TEXTURE2D_DESC& desc);
		virtual BOOL Create(DX11& dx11, ID3D11Texture2D* texture2D);
		virtual BOOL Create(DX11& dx11, INT cx, INT cy, DXGI_FORMAT dxgiFormat, const BYTE* bits, BOOL bReadoly);//B8G8R8A8
		virtual BOOL Create(DX11& dx11, INT cx, INT cy, DXGI_FORMAT dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM);//B8G8R8A8
		virtual BOOL Load(DX11& dx11, const BYTE* bits, LONG size);
		virtual BOOL Load(DX11& dx11, HANDLE hResource);
		virtual BOOL Load(DX11& dx11, const CHAR* pzFile);
		virtual BOOL Copy(DX11& dx11, ID3D11Texture2D* texture2D);
		virtual BOOL Copy(DX11& dx11, DX11Texture2D& texture2D);
		virtual BOOL Copy(DX11& dx11, D3D11_BOX* ps, const BYTE* bits, LONG size, UINT rowPitch, UINT depthPitch);
		virtual BOOL SaveAs(DX11& dx11, const CHAR* pzFile, IMAGE_FILE_FORMAT dxgi);
		virtual void Destory();
	public:
		operator ID3D11Texture2D*() const;
		ID3D11Texture2D*			GetTexture2D() const;
		BOOL						GetDC(BOOL discard, HDC& hDC);
		BOOL						ReleaseDC();
		BOOL						Map(DX11& dx11, D3D11_MAPPED_SUBRESOURCE& ms, BOOL bReadoly = FALSE);
		void						Unmap(DX11& dx11);
		HANDLE						GetHandle();//获得共享句柄
		BOOL						IsEmpty() const;
		ID3D11ShaderResourceView*	GetSRView() const;
	protected:
		TinyComPtr<IDXGISurface1>				m_surface;
		TinyComPtr<ID3D11Texture2D>				m_texture2D;
		TinyComPtr<ID3D11ShaderResourceView>	m_resourceView;
	};
}

