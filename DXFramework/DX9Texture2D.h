#pragma once
#include "DX9.h"

namespace DXFramework
{
	class DX9Texture2D
	{
	public:
		DX9Texture2D();
		virtual ~DX9Texture2D();
		virtual BOOL Load(DX9& dx9, const CHAR* pzFile);
		virtual BOOL Load(DX9& dx9, const BYTE* bits, LONG size);
		virtual BOOL Create(DX9& dx9, INT cx, INT cy, const BYTE* bits);
	public:
		BOOL	IsEmpty() const;
	private:
		TinyComPtr<IDirect3DTexture9>	m_texture2D;
	};
}


