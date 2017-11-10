#pragma once
#include "DXFramework.h"
#include <Richedit.h>
#include <d3dx9core.h>

namespace DXFramework
{
	/// <summary>
	/// DX9×ÖÌå
	/// </summary>
	class DX9Font2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX9Font2D)
	public:
		DX9Font2D();
		virtual ~DX9Font2D();
	private:
		TinyComPtr<ID3DXFont>	m_d3dFont;
	};
}



