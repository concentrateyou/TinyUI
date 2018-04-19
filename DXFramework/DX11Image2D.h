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
		};
		DECLARE_DYNAMIC(DX11Image2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Image2D)
	public:
		DX11Image2D();
		virtual ~DX11Image2D();
		BOOL Translate(DX11& dx11, FLOAT ratioX = 1.0F, FLOAT ratioY = 1.0F);
	public:
		BOOL Create(DX11& dx11, ID3D11Texture2D* texture2D) OVERRIDE;
		BOOL Create(DX11& dx11, INT cx, INT cy, const BYTE* bits, BOOL bReadoly) OVERRIDE;
		BOOL Create(DX11& dx11, INT cx, INT cy, BOOL bMutex = FALSE) OVERRIDE;
		BOOL Load(DX11& dx11, const BYTE* bits, LONG dwSize) OVERRIDE;
		BOOL Load(DX11& dx11, HANDLE hResource) OVERRIDE;
		BOOL Load(DX11& dx11, const CHAR* pzFile) OVERRIDE;
		void Destory() OVERRIDE;
	public:
		BOOL Copy(DX11& dx11, D3D11_BOX* ps, const BYTE* bits, LONG size);
		BOOL Copy(DX11& dx11, const BYTE* bits, LONG size, UINT stride);
		BOOL BitBlt(DX11& dx11, const BYTE* bits, LONG size, LONG linesize);
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src);
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src);
	public:
		virtual INT	GetVertexCount() const;
		virtual INT	GetIndexCount() const;
	public:
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Process(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		BOOL Initialize(DX11& dx11);
	protected:
		TinySize					m_size;
		TinyComPtr<ID3D11Buffer>	m_vertexs;
		TinyComPtr<ID3D11Buffer>	m_indexs;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}

