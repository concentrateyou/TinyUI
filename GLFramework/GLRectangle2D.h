#pragma once
#include "GL.h"
#include "GLElement2D.h"

namespace GLFramework
{
	class GLRectangle2D : public GLElement2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(GLRectangle2D)
		DISALLOW_COPY_AND_ASSIGN(GLRectangle2D)
	public:
		GLRectangle2D();
		virtual ~GLRectangle2D();
		BOOL	Create(GL& gl);
		void	Destory();
	public:
		BOOL	DrawRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL	FillRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color);
	private:
		GLuint	m_vertexArrayID;
		GLuint	m_vertexID;
		GLuint	m_indexs[2];
	};
}


