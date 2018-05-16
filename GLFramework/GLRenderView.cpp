#include "stdafx.h"
#include "GLRenderView.h"

namespace GLFramework
{
	GLRenderView::GLRenderView(GL& gl)
		:m_gl(gl)
	{
	}

	GLRenderView::~GLRenderView()
	{
	}
	GL&	GLRenderView::GetGL()
	{
		return m_gl;
	}
	BOOL GLRenderView::Create(INT cx, INT cy)
	{

		return TRUE;
	}
	BOOL GLRenderView::Resize(INT cx, INT cy)
	{
		return TRUE;
	}
	BOOL GLRenderView::BeginDraw()
	{
		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return TRUE;
	}
	BOOL GLRenderView::EndDraw()
	{
		return SwapBuffers(m_gl.GetDC());
	}
}