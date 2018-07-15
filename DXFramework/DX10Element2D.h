#pragma once
#include "DXFramework.h"
#include "DX10Texture2D.h"
#include "DX10TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyFramework;

namespace DXFramework
{
	/// <summary>
	/// DX10ÔªËØ
	/// </summary>
	class DX10Element2D : public TinyObject
	{
		DECLARE_DYNAMIC(DX10Element2D)
	public:
		DX10Element2D();
		virtual ~DX10Element2D();
	public:
		TinyString		GetElementName() const;
		void			SetElementName(const TinyString& name);
	protected:
		TinyString		m_szElement;
	};
}



