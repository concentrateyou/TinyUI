#pragma once
#include "GL.h"
#include "GLTexture2D.h"
#include "GLImageElement2D.h"

namespace GLFramework
{
	class GLImage2D : public GLImageElement2D, public GLTexture2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(GLImage2D)
		DISALLOW_COPY_AND_ASSIGN(GLImage2D)
	public:
		GLImage2D();
		virtual ~GLImage2D();
	public:
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
		VERTEXTYPE		m_vertexTypes[4];
		UINT			m_indices[6];
		GLuint			m_vertexArrayID;
		GLuint			m_vertexID;
		GLuint			m_indexID;
	};
}