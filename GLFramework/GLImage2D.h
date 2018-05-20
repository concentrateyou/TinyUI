#pragma once
#include "GLTexture2D.h"
#include "GLImageElement2D.h"

namespace GLFramework
{
	class GLImage2D : public GLTexture2D, public GLImageElement2D
	{
	private:
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DISALLOW_COPY_AND_ASSIGN(GLImage2D)
	public:
		GLImage2D();
		virtual ~GLImage2D();
		BOOL	Create(GL& gl, INT cx, INT cy, const BYTE* bits) OVERRIDE;
		BOOL	Load(GL& gl, const BYTE* bits, LONG size) OVERRIDE;
		BOOL	Load(GL& gl, const CHAR* pzFile) OVERRIDE;
		void	Destory() OVERRIDE;
	public:
		BOOL	DrawImage(GL& gl);
	private:
		BOOL	Initialize(GL& gl);
		BOOL	Calculate(GL& gl);
	private:
		GLuint	m_vertexArrayID;
		GLuint	m_vertexID;
		GLuint	m_indexID;
	};
}