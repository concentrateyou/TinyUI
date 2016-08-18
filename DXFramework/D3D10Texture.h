#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class D3D10Texture
	{
		DISALLOW_COPY_AND_ASSIGN(D3D10Texture)
	public:
		D3D10Texture();
		~D3D10Texture();
		BOOL Initialize(ID3D10Device* pDevice, CHAR* pzFile);
		BOOL Initialize(ID3D10Device* pDevice, HANDLE hResource);
		TinySize GetSize() const;
		ID3D10ShaderResourceView*  GetTexture() const;
	private:
		TinyComPtr<ID3D10ShaderResourceView>	m_texture;
		TinySize	m_size;
	};
}

