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
		m_camera.UpdatePosition();
		return TRUE;
	}

	BOOL GLGraphics2D::BeginDraw()
	{
		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return TRUE;
	}
	BOOL GLGraphics2D::EndDraw()
	{
		return TRUE;
	}
	BOOL GLGraphics2D::Present()
	{
		return SwapBuffers(m_gl.GetDC());
	}
	BOOL GLGraphics2D::DrawImage(GLImage2D& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_gl))
			return FALSE;
		XMMATRIX* ms = m_gl.GetMatrixs();
		m_textureShader.SetShaderParameters(m_gl, ms[1], m_camera.GetView(), ms[2], image.GetTexture2D());
		m_textureShader.Render(m_gl);
		return TRUE;
	}
}
