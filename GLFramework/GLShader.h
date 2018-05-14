#pragma once
#include "GLFramework.h"
#include "GL.h"

namespace GLFramework
{
	class GLShader
	{
		DISALLOW_COPY_AND_ASSIGN(GLShader)
	public:
		GLShader();
		virtual ~GLShader();
		virtual BOOL Initialize(GL& gl, const CHAR* vsFile, const CHAR* psFile) = 0;
		virtual void Render(GL& gl) = 0;
	};

}

