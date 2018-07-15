#pragma once
#include "DX10Texture2D.h"

namespace DXFramework
{
	class DX10Shader
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Shader)
	public:
		DX10Shader();
		virtual ~DX10Shader();
	public:
		virtual BOOL Initialize(DX10& dx10, const CHAR* vsFile, const CHAR* psFile) = 0;
		virtual void Render(DX10& dx10) = 0;
	};
}


