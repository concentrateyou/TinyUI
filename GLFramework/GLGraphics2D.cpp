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
	GL&	GLGraphics2D::GetGL()
	{
		return m_gl;
	}
	BOOL GLGraphics2D::Initialize(HWND hWND, const TinySize& size)
	{
		if (!m_gl.Initialize(hWND, size.cx, size.cy))
			return FALSE;

		string str;
		str.resize(MAX_PATH);
		GetModuleFileName(NULL, &str[0], MAX_PATH);
		str = str.substr(0, str.find_last_of("\\", string::npos, 1));
		string vs = str + "\\gltexture.vs";
		ASSERT(PathFileExists(vs.c_str()));
		string ps = str + "\\gltexture.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_textureShader.Initialize(m_gl, vs.c_str(), ps.c_str()))
			return FALSE;

		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		return TRUE;
	}
}
