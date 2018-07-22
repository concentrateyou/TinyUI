#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyFramework;

namespace DXFramework
{
	/// <summary>
	/// DX11ÔªËØ
	/// </summary>
	class DX11Element : public TinyObject
	{
		DECLARE_DYNAMIC(DX11Element)
	public:
		DX11Element();
		virtual ~DX11Element();
	public:
		TinyString		GetElementName() const;
		void			SetElementName(const TinyString& name);
	protected:
		TinyString		m_szElement;
	};
}



