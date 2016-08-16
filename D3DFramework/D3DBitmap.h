#pragma once
#include "Common.h"
#include "D3D10Texture.h"

namespace DX
{
	struct VERTEXTYPE
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
	class D3DBitmap
	{
	public:
		D3DBitmap();
		~D3DBitmap();
		BOOL Initialize(ID3D10Device* device, INT cx, INT cy, CHAR* pzFile);
		BOOL Initialize(ID3D10Device* device, INT cx, INT cy, HANDLE hResource);
		BOOL Uninitialize();
		BOOL Render(ID3D10Device* device, INT, INT);
		ID3D10ShaderResourceView* GetTexture();
		BOOL LoadTexture(ID3D10Device*, CHAR* pzFile);
		BOOL LoadTexture(ID3D10Device* device, HANDLE hResource);
	private:
		BOOL InitializeBuffers(ID3D10Device* device);
	private:
		D3D10Texture			 m_texture;
		TinyComPtr<ID3D10Buffer> m_vertexBuffer;
		TinyComPtr<ID3D10Buffer> m_indexBuffer;
		INT m_vertexCount;
		INT m_indexCount;
		INT m_previousPosX;
		INT m_previousPosY;
		INT m_cx;
		INT m_cy;
	};
}

