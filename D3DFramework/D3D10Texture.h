#pragma once
#include "Common.h"

namespace DX
{
	class D3D10Texture
	{
		DISALLOW_COPY_AND_ASSIGN(D3D10Texture)
	public:
		D3D10Texture();
		~D3D10Texture();
		BOOL Initialize(ID3D10Device*, HANDLE);
		ID3D10ShaderResourceView*  GetTexture() const;
	private:
		TinyComPtr<ID3D10ShaderResourceView> m_texture;
	};
}

