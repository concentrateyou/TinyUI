#pragma once
#include "GLElement2D.h"
#include "GL.h"
#include "Render/TinyImage.h"
using namespace TinyUI;

namespace GLFramework
{
	/// <summary>
	/// OpenGL 2D纹理只支持RGB32
	/// </summary>
	class GLTexture2D : public GLElement2D
	{
		DISALLOW_COPY_AND_ASSIGN(GLTexture2D)
	public:
		GLTexture2D();
		virtual ~GLTexture2D();
		virtual BOOL Create(GL& gl, INT cx, INT cy, const BYTE* bits);
		virtual BOOL Load(GL& gl, const BYTE* bits, LONG size);
		virtual BOOL Load(GL& gl, const CHAR* pzFile);
		virtual void Destory(GL& gl);
	public:
		operator GLuint() const;
		GLuint	GetTexture2D() const;
	protected:
		GLuint		m_textureID;
		TinyImage	m_image;
	};
}



