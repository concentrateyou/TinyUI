#include "stdafx.h"
#include "GLGraphics2D.h"

namespace GLFramework
{
	GLGraphics2D::GLGraphics2D()
	{
	}


	GLGraphics2D::~GLGraphics2D()
	{
	}

	BOOL GLGraphics2D::Initialize(HWND hWND, INT cx, INT cy)
	{
		if (!m_ogl.Initialize(hWND, cx, cy))
			return FALSE;

		return TRUE;
	}
	BOOL GLGraphics2D::BeginDraw()
	{
		return m_ogl.BeginDraw();
	}
	BOOL GLGraphics2D::EndDraw()
	{
		return m_ogl.EndDraw();
	}
}
