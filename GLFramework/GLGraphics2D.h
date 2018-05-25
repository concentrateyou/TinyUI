#pragma once
#include "GL.h"
#include "GLCamera.h"

namespace GLFramework
{
	class GLGraphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(GLGraphics2D)
	public:
		GLGraphics2D();
		~GLGraphics2D();
	public:
		BOOL		Initialize(HWND hWND, INT cx, INT cy);
		BOOL		BeginDraw();
		BOOL		EndDraw();
	private:
		GL			m_ogl;
		GLCamera	m_camera;
	};
}

