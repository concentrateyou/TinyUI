#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	class DX11Image
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Image)
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};
	public:
		DX11Image();
		~DX11Image();
		void SetPosition(INT cx, INT cy);
		BOOL Create(const DX11& dx11, INT cx, INT cy, INT scaleX, INT scaleY);
		BOOL Fill(const DX11& dx11, const BYTE* pData);
		BOOL BitBlt(const DX11& dx11, const BYTE* pData);
		BOOL Load(const DX11& dx11, HANDLE hResource, INT scaleX, INT scaleY);
		BOOL Load(const DX11& dx11, const CHAR* pzFile, INT scaleX, INT scaleY);
		BOOL Load(const DX11& dx11, const BYTE* data, DWORD dwSize, INT scaleX, INT scaleY);
		BOOL Render(const DX11& dx11, INT positionX, INT positionY);
		INT	 GetIndexCount() const;
		DX11Texture* GetTexture();
		BOOL IsValid() const;
	private:
		BOOL Initialize(const DX11& dx11);
		BOOL Update(const DX11& dx11, INT positionX, int positionY);
	private:
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		INT							m_scaleX;
		INT							m_scaleY;
		INT							m_positionX;
		INT							m_positionY;
		INT							m_vertexCount;
		INT							m_indexCount;
		DX11Texture					m_texture;
		
	};
	/// <summary>
	/// π≤œÌŒ∆¿Ì
	/// </summary>
	class SharedTexture
	{
	public:
		SharedTexture();
		~SharedTexture();
		BOOL Initialize(const DX11& dx10, INT scaleX, INT scaleY);
		DX11Image& GetTexture();
	private:
		DX11Image				m_image;
		IO::TinySharedMemory	m_textureMemery;
		HWND					m_hWND;
	};
}

