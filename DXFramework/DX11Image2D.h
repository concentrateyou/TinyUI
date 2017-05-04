#pragma once
#include "DX11Element2D.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Image2D : public DX11Element2D
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
		BOOL Create(DX11& dx11, ID3D11Texture2D* texture2D);
		BOOL Create(DX11& dx11, const TinySize& size, BYTE* bits = NULL, BOOL bReadonly = TRUE);
		BOOL CreateCompatible(DX11& dx11, const TinySize& size);
		BOOL BitBlt(DX11& dx11, const BYTE* bits, LONG size, LONG linesize);//RGB32
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src);
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src);
		BOOL Copy(DX11& dx11, ID3D11Texture2D* texture2D);
		BOOL Copy(DX11& dx11, const BYTE* bits, LONG size, LONG linesize);
		BOOL SaveAs(DX11& dx11, const CHAR* pzName);
		BOOL GetDC(BOOL discard, HDC& hDC);
		BOOL ReleaseDC();
		BOOL Map(DX11& dx11, BYTE *&lpData, UINT &pitch);
		void Unmap(DX11& dx11);
		BOOL Load(DX11& dx11, HANDLE hResource);
		BOOL Load(DX11& dx11, const CHAR* pzFile);
		BOOL Load(DX11& dx11, const BYTE* bits, DWORD dwSize);
		BOOL IsEmpty() const;
		BOOL Update(DX11& dx11);
		DX11Texture2D* GetTexture();
	public:
		virtual INT	GetIndexCount() const;
		virtual void Destory();
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
		void Clear(DX11& dx11) OVERRIDE;
	private:
		BOOL Initialize(DX11& dx11);
	protected:
		DX11Texture2D				m_texture;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}

