#pragma once
#include "DX11Element.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Image : public DX11Element
	{
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};
		DECLARE_DYNAMIC(DX11Image)
		DISALLOW_COPY_AND_ASSIGN(DX11Image)
	public:
		DX11Image();
		virtual ~DX11Image();
		BOOL Create(DX11& dx11, const TinySize& size, BYTE* bits = NULL, BOOL bReadonly = TRUE);
		BOOL CreateCompatible(DX11& dx11, const TinySize& size, BYTE* bits = NULL);
		BOOL BitBlt(DX11& dx11, const BYTE* bits, LONG size);//RGB32
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src);
		BOOL BitBlt(DX11& dx11, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src);
		BOOL Copy(DX11& dx11, const BYTE* bits, LONG size);
		BOOL Copy(DX11& dx11, const BYTE* bits, UINT pitch);
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
		TinyPoint					m_lastPos;
		TinySize					m_lastScale;
		DX11Texture2D				m_texture;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}

