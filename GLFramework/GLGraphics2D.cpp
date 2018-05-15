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

	BOOL GLGraphics2D::Initialize(HWND hWND, const TinySize& size)
	{
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		return TRUE;
	}
}
