#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "DX11Image2D.h"

namespace DXFramework
{
	/// <summary>
	/// DX11ЪѓБъ
	/// </summary>
	class DX11Cursor2D : public DX11Image2D
	{
		DECLARE_DYNAMIC(DX11Cursor2D)
	public:
		DX11Cursor2D();
		virtual ~DX11Cursor2D();
		HICON	GetCursor() const;
		BOOL	Create(DX11& dx11, HICON hICON);
	private:
		BOOL	GetCursor(HICON hICON, SIZE& size);
		BOOL	AlphaPixels(const UINT32* pixel, UINT32 count);
		BOOL	UpdateCursor(DX11& dx11, HICON hICON, const SIZE& size);
	private:
		HICON	m_hICON;
	};
}



