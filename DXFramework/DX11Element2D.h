#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
	/// <summary>
	/// DX11ÔªËØ
	/// </summary>
	class DX11Element2D : public TinyObject
	{
		DECLARE_DYNAMIC(DX11Element2D)
	public:
		DX11Element2D();
		virtual ~DX11Element2D();
	public:
		TinyString		GetElementName() const;
		void			SetElementName(const TinyString& name);
	protected:
		TinyString		m_szElement;
	};
}



