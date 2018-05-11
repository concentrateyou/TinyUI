#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11Shader
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Shader)
	public:
		DX11Shader();
		virtual ~DX11Shader();
	public:
		virtual BOOL Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile) = 0;
		virtual void Render(DX11& dx11) = 0;
	};
}


