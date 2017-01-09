#pragma once
#include "DX11Element.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	class DX11Image : public DX11Element
	{
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};
		DISALLOW_COPY_AND_ASSIGN(DX11Image)
	public:
		DX11Image();
		virtual ~DX11Image();
		BOOL Create(const DX11& dx11, const TinySize& size, BYTE* bits = NULL);
		BOOL BitBlt(const DX11& dx11, const BYTE* bits);//RGB32
		BOOL Load(const DX11& dx11, HANDLE hResource);
		BOOL Load(const DX11& dx11, const CHAR* pzFile);
		BOOL Load(const DX11& dx11, const BYTE* bits, DWORD dwSize);
		BOOL IsValid() const;
		BOOL Update(const DX11& dx11);
		DX11Texture* GetTexture();
	public:
		ElementType	GetElementType() const;
		LPCSTR GetClassName() OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(const DX11& dx11) OVERRIDE;
		INT GetIndexCount() const OVERRIDE;
	private:
		BOOL Initialize(const DX11& dx11);
	private:
		DX11Texture					m_texture;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
	};
}

