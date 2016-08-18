#pragma once
#include "D3D10Texture.h"

namespace DXFramework
{
	struct VERTEXTYPE
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
	class D3D10Bitmap
	{
	public:
		D3D10Bitmap();
		~D3D10Bitmap();
		BOOL Initialize(ID3D10Device* pDevice, INT cx, INT cy, CHAR* pzFile);
		BOOL Initialize(ID3D10Device* pDevice, INT cx, INT cy, HANDLE hResource);
		BOOL Render(ID3D10Device* pDevice, INT, INT);
		BOOL LoadTexture(ID3D10Device* pDevice, CHAR* pzFile);
		BOOL LoadTexture(ID3D10Device* pDevice, HANDLE hResource);
		ID3D10ShaderResourceView* GetTexture();
	private:
		BOOL InitializeBuffers(ID3D10Device* pDevice);
		BOOL UpdateBuffers(INT positionX, INT positionY);
		void RenderBuffers(ID3D10Device* pDevice);
	private:
		D3D10Texture				m_texture;
		TinyComPtr<ID3D10Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D10Buffer>	m_indexBuffer;
		INT							m_previousPosX;
		INT							m_previousPosY;
		INT							m_cx;
		INT							m_cy;
	};
}

