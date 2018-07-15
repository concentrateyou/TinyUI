#pragma once
#include "DXFramework.h"
#include "DX10.h"
#include "WICTexture.h"

namespace DXFramework
{
	/// <summary>
	/// 2D纹理
	/// </summary>
	class DX10Texture2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Texture2D)
	public:
		DX10Texture2D();
		DX10Texture2D(ID3D11Texture2D* texture2D);
		virtual ~DX10Texture2D();
	public:
		virtual BOOL Create(DX10& dx10, D3D10_TEXTURE2D_DESC& desc);
		virtual BOOL Create(DX10& dx10, ID3D10Texture2D* texture2D);
		virtual BOOL Create(DX10& dx10, INT cx, INT cy, const BYTE* bits, BOOL bReadoly);//B8G8R8A8
		virtual BOOL Create(DX10& dx10, INT cx, INT cy);//B8G8R8A8
		virtual BOOL Load(DX10& dx10, const BYTE* bits, LONG size);
		virtual BOOL Load(DX10& dx10, HANDLE hResource);
		virtual BOOL Load(DX10& dx10, const CHAR* pzFile);
		virtual BOOL Copy(DX10& dx10, ID3D10Texture2D* texture2D);
		virtual BOOL Copy(DX10& dx10, DX10Texture2D& texture2D);
		virtual BOOL Copy(DX10& dx10, D3D10_BOX* ps, const BYTE* bits, LONG size, UINT rowPitch, UINT depthPitch);
		virtual BOOL SaveAs(DX10& dx10, const CHAR* pzFile, IMAGE_FILE_FORMAT format);
		virtual void Destory();
	public:
		operator ID3D10Texture2D*() const;
		ID3D10Texture2D*			GetTexture2D() const;
		BOOL						GetDC(BOOL discard, HDC& hDC);
		BOOL						ReleaseDC();
		BOOL						Map(D3D10_MAPPED_TEXTURE2D& ms, BOOL bReadoly = FALSE);
		void						Unmap();
		HANDLE						GetHandle();//获得共享句柄
		BOOL						IsEmpty() const;
		ID3D10ShaderResourceView*	GetSRView() const;
	protected:
		TinyComPtr<IDXGISurface1>				m_surface;
		TinyComPtr<ID3D10Texture2D>				m_texture2D;
		TinyComPtr<ID3D10ShaderResourceView>	m_resourceView;
	};
}

