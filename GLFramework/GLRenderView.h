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
		BOOL		BeginDraw();
		BOOL		EndDraw();
	private:
		GL&			m_gl;
	};
}


