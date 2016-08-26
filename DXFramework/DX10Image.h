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
		BOOL Create(const DX10& dx10, INT cx, INT cy, INT scaleX, INT scaleY);
		BOOL FillImage(const BYTE* pBits, INT cx, INT cy);
		BOOL Load(const DX10& dx10, HANDLE hResource, INT scaleX, INT scaleY);
		BOOL Load(const DX10& dx10, const CHAR* pzFile, INT scaleX, INT scaleY);
		BOOL Load(const DX10& dx10, const BYTE* pData, INT size, INT scaleX, INT scaleY);
		BOOL Render(const DX10& dx10, INT positionX, INT positionY);
		INT	 GetIndexCount() const;
		DX10Texture* GetTexture();
	private:
		BOOL Initialize(const DX10& dx10);
		BOOL Update(const DX10& dx10, INT positionX, int positionY);
	private:
		TinyComPtr<ID3D10Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D10Buffer>	m_indexBuffer;
		INT							m_scaleX;
		INT							m_scaleY;
		INT							m_previousPosX;
		INT							m_previousPosY;
		INT							m_vertexCount;
		INT							m_indexCount;
		DX10Texture					m_texture;
	};
}

