#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"
#include "DX11Image.h"

namespace DXFramework
{
	/// <summary>
	/// DX11×ÖÌå
	/// </summary>
	class DX11Font : public DX11Image
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Font)
	public:
		DX11Font();
		virtual ~DX11Font();
		BOOL Create(DX11& dx11, HFONT hFont, const wstring& str);
	};
}



