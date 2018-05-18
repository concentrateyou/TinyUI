#pragma once
#include "GL.h"

namespace GLFramework
{
	class GLRenderView
	{
		DISALLOW_COPY_AND_ASSIGN(GLRenderView)
	public:
		GLRenderView(GL& gl);
		~GLRenderView();
		GL&			GetGL();
		BOOL		Create(INT cx, INT cy);
		BOOL		Resize(INT cx, INT cy);
		void		Destory();
		BOOL		BeginDraw();
		BOOL		EndDraw();
	private:
		GL&			m_gl;
		GLuint		m_depthBuffers;
		GLuint		m_frameBuffers;
		GLuint		m_textureID;
	};
}


