#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "DX11Image2D.h"

namespace DXFramework
{
	/// <summary>
	/// DX11×ÖÌå
	/// </summary>
	class DX11Font2D : public DX11Image2D
	{
		DECLARE_DYNAMIC(DX11Font2D)
	public:
		DX11Font2D();
		virtual ~DX11Font2D();
		BOOL Create(DX11& dx11, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor);
		BOOL DrawString(DX11& dx11, const TinyString& str, const RectF& rectF, const StringFormat* format);
		BOOL ClearContext();
	private:
		CHARFORMAT	m_cf;
		COLORREF	m_textColor;
		COLORREF	m_bkColor;
	};
}



