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
		DX11Texture* GetTexture()
	public:
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(const DX11& dx11) OVERRIDE;
	private:
		BOOL Initialize(const DX11& dx11);
	private:
		DX11Texture					m_texture;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
	};
	/// <summary>
	/// π≤œÌŒ∆¿Ì
	/// </summary>
	class SharedTexture
	{
	public:
		SharedTexture();
		~SharedTexture();
		BOOL Initialize(const DX11& dx10, const TinySize& scale);
		DX11Image& GetTexture();
	private:
		DX11Image				m_image;
		IO::TinySharedMemory	m_textureMemery;
		HWND					m_hWND;
	};
}

