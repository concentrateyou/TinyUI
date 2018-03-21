#pragma once
#include "GLElement2D.h"
#include "GL.h"

namespace GLFramework
{
	class GLTexture2D : public GLElement2D
	{
		DISALLOW_COPY_AND_ASSIGN(GLTexture2D)
	public:
		GLTexture2D();
		virtual ~GLTexture2D();
		virtual BOOL Create(GL& gl, INT cx, INT cy, const BYTE* bits);
		virtual BOOL Load(GL& gl, const BYTE* bits, LONG size);
		virtual BOOL Load(GL& gl, const CHAR* pzFile);
		virtual void Destory();
	private:
		GLuint m_textureID;
	};
}



