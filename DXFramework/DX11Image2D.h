#pragma once
#include "DX11Element2D.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Image2D : public DX11Element2D, public DX11Texture2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11Image2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Image2D)
	public:
		DX11Image2D();
		virtual ~DX11Image2D();
	public:
		FLOAT		GetRotate() const;
		XMFLOAT2	GetSize() const;
		XMFLOAT2	GetScale() const;
		XMFLOAT2	GetTranslate() const;
		BOOL		IsFlipH() const;
		BOOL		IsFlipV() const;
		void		SetSize(const XMFLOAT2& size);
		void		SetRotate(FLOAT angle);
		void		SetScale(const XMFLOAT2& scale);
		void		SetTranslate(const XMFLOAT2& pos);
		void		SetFlipH(BOOL bFlag);
		void		SetFlipV(BOOL bFlag);
	public:
		BOOL		Create(DX11& dx11, ID3D11Texture2D* texture2D) OVERRIDE;
		BOOL		Create(DX11& dx11, INT cx, INT cy, const BYTE* bits, BOOL bReadoly) OVERRIDE;
		BOOL		Create(DX11& dx11, INT cx, INT cy) OVERRIDE;
		BOOL		Load(DX11& dx11, const BYTE* bits, LONG dwSize) OVERRIDE;
		BOOL		Load(DX11& dx11, HANDLE hResource) OVERRIDE;
		BOOL		Load(DX11& dx11, const CHAR* pzFile) OVERRIDE;
		void		Destory() OVERRIDE;
		BOOL		DrawImage(DX11& dx11);
	public:
		BOOL		Copy(DX11& dx11, D3D11_BOX* ps, const BYTE* bits, LONG size);
		BOOL		Copy(DX11& dx11, const BYTE* bits, LONG size, UINT stride);
		BOOL		BitBlt(DX11& dx11, const BYTE* bits, LONG size, LONG linesize);
		BOOL		BitBlt(DX11& dx11, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src);
		BOOL		BitBlt(DX11& dx11, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src);
	private:
		BOOL		Initialize(DX11& dx11);
		BOOL		Calculate(DX11& dx11);
	protected:
		BOOL						m_bFlipH;
		BOOL						m_bFlipV;
		FLOAT						m_angle;
		XMFLOAT2					m_size;//元素大小
		XMFLOAT2					m_scale;//缩放比例
		XMFLOAT2					m_translate;//平移坐标
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_indexs;
	};
}

