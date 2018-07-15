#pragma once
#include "DXFramework.h"
#include "DX10Texture2D.h"
#include "DX10TextureShader.h"
#include "DX10Image2D.h"
#include <Richedit.h>

namespace DXFramework
{
	/// <summary>
	/// DX10×ÖÌå
	/// </summary>
	class DX10Font2D : public DX10Image2D
	{
		DECLARE_DYNAMIC(DX10Font2D)
	public:
		DX10Font2D();
		virtual ~DX10Font2D();
		BOOL Create(DX10& dx10, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor);
		BOOL DrawString(DX10& dx10, const TinyString& str, const RectF& rectF, const StringFormat* format);
		BOOL ClearContext();
	private:
		CHARFORMAT	m_cf;
		COLORREF	m_textColor;
		COLORREF	m_bkColor;
	};
}



