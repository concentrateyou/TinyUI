#pragma once
#include "GLFramework.h"
#include "GL.h"
#include "GLShader.h"

namespace GLFramework
{
	class GLTextureShader : public GLShader
	{
		DISALLOW_COPY_AND_ASSIGN(GLTextureShader)
	public:
		GLTextureShader();
		virtual ~GLTextureShader();
	public:
		BOOL Initialize(GL& gl, const CHAR* vsFile, const CHAR* psFile) OVERRIDE;
		void Render(GL& gl) OVERRIDE;

	};
}

