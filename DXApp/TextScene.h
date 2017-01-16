#pragma once
#include "DX11.h"
#include "DX11Font.h"
#include "Common/TinyString.h"
#include <Richedit.h>
using namespace TinyUI;
using namespace DXFramework;

namespace DXApp
{
	class TextScene : public DX11Font
	{
	public:
		TextScene();
		virtual ~TextScene();
		BOOL Initialize(DX11& dx11, const TinyString& text, const CHARFORMAT& cf, const COLORREF& bkColor);
	public:
		LPCSTR GetClassName() OVERRIDE;
		ElementType GetElementType() const OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
	private:
		TinyString		m_text;
		COLORREF		m_bkColor;
		CHARFORMAT		m_cf;
		TinyScopedPtr<Gdiplus::Font> m_font;
	};
}



