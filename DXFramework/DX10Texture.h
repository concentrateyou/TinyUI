#pragma once
#include "DXFramework.h"
#include "DX10.h"

namespace DXFramework
{
#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8) 
#endif
	/// <summary>
	/// Œ∆¿Ì
	/// </summary>
	class DX10Texture
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Texture)
	public:
		DX10Texture();
		~DX10Texture();
		BOOL CreateTexture(const DX10& dx10, INT cx, INT cy);
		BOOL FillTexture(const BYTE* pBits, INT cx, INT cy);
		BOOL Save(const CHAR* pzFile, D3DX10_IMAGE_FILE_FORMAT format);
		BOOL LoadTexture(const DX10& dx10, const BYTE* pBits, INT size);
		BOOL LoadTexture(const DX10& dx10, HANDLE hResource);
		BOOL LoadTexture(const DX10& dx10, const CHAR* pzFile);
		ID3D10Texture2D* GetTexture2D() const;
		ID3D10ShaderResourceView* GetSRView() const;
		BOOL IsValid() const;
		TinySize GetSize();
	private:
		TinyComPtr<ID3D10Texture2D>				m_texture2D;
		TinyComPtr<ID3D10ShaderResourceView>	m_resourceView;
	};
}

