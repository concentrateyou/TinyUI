#pragma once
#include "DX9.h"
#include <d3dx9core.h>

namespace DXFramework
{
	/// <summary>
	/// DX9×ÅÉ«Æ÷
	/// </summary>
	class DX9TextureShader
	{
		DISALLOW_COPY_AND_ASSIGN(DX9TextureShader)
	public:
		DX9TextureShader();
		virtual ~DX9TextureShader();
		BOOL Initialize(DX9& dx9, const CHAR* szFile);
	};
}



