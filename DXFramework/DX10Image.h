#pragma once
#include "DXFramework.h"
#include "DX10Texture.h"
#include "DX10TextureShader.h"

namespace DXFramework
{
	class DX10Image
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Image);
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};
	public:
		DX10Image();
		~DX10Image();
		BOOL Load(const DX10& dx10, HANDLE hResource);
		BOOL Update(const DX10& dx10, INT positionX, int positionY);
	private:
		BOOL Initialize(const DX10& dx10);
	private:
		TinyComPtr<ID3D10Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D10Buffer>	m_indexBuffer;
		INT							m_previousPosX;
		INT							m_previousPosY;
		INT							m_vertexCount;
		INT							m_indexCount;
		DX10Texture					m_dx10Texture;
	};
}

