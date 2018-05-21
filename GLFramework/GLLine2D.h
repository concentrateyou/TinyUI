#pragma once
#include "GL.h"
#include "GLElement2D.h"

namespace GLFramework
{
	class GLLine2D : public GLElement2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(GLLine2D)
		DISALLOW_COPY_AND_ASSIGN(GLLine2D)
	public:
		GLLine2D();
		virtual ~GLLine2D();
		BOOL	Create(GL& gl);
		void	Destory();
	public:
		BOOL	DrawLine(GL& gl, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
	private:
		GLuint					m_vertexArrayID;
		GLuint					m_vertexID;
		GLuint					m_indexID;
		TinyBuffer<VERTEXTYPE>	m_vertexTypes;
	};
}


