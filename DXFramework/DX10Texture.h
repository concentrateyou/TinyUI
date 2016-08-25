#pragma once
#include "DXFramework.h"
#include "DX10.h"

namespace DXFramework
{
	/// <summary>
	/// Œ∆¿Ì
	/// </summary>
	class DX10Texture
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Texture)
	public:
		DX10Texture();
		~DX10Texture();
		BOOL LoadTexture(const DX10& dx10, HANDLE hResource);
		BOOL LoadTexture(const DX10& dx10, const CHAR* pzFile);
		BOOL LoadTexture(const DX10& dx10, const BYTE* data, INT size);
		ID3D10Texture2D* GetTexture2D() const;
		ID3D10ShaderResourceView* GetSRView() const;
		SIZE GetSize();
	private:
		TinyComPtr<ID3D10Texture2D>				m_texture2D;
		TinyComPtr<ID3D10ShaderResourceView>	m_view;
	};
}

