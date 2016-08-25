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
		BOOL Load(const DX10& dx10, const CHAR* pzFile);
		BOOL Load(const DX10& dx10, const BYTE* pData, INT size);
		BOOL Render(const DX10& dx10, INT positionX, INT positionY);
		INT	 GetIndexCount() const;
		ID3D10Texture2D* GetTexture2D() const;
	private:
		BOOL Initialize(const DX10& dx10);
		BOOL Update(const DX10& dx10, INT positionX, int positionY);
	private:
		TinyComPtr<ID3D10Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D10Buffer>	m_indexBuffer;
		INT							m_previousPosX;
		INT							m_previousPosY;
		INT							m_vertexCount;
		INT							m_indexCount;
		DX10Texture					m_texture;
	};
}

