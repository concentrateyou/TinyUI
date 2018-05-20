#pragma once
#include "GL.h"
#include "GLElement2D.h"

namespace GLFramework
{
	class GLTexture2D
	{
		DISALLOW_COPY_AND_ASSIGN(GLTexture2D)
	public:
		GLTexture2D();
		virtual ~GLTexture2D();
		virtual BOOL Create(GL& gl, INT cx, INT cy, const BYTE* bits);
		virtual BOOL Load(GL& gl, const BYTE* bits, LONG size);
		virtual BOOL Load(GL& gl, const CHAR* pzFile);
		virtual void Destory();
	public:
		operator GLuint() const;
		BOOL	IsEmpty() const;
		GLuint	GetID() const;
	protected:
		GLuint		m_textureID;
		TinyImage	m_image;
	};
}