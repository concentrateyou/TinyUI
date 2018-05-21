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

	GL& GLGraphics2D::GetGL()
	{
		return m_gl;
	}

	BOOL GLGraphics2D::Initialize(HWND hWND, INT cx, INT cy)
	{
		if (!m_gl.Initialize(hWND, cx, cy))
			return FALSE;

		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		m_camera.UpdateView();

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

		vs = str + "\\glcolor.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\glcolor.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_colorSharder.Initialize(m_gl, vs.c_str(), ps.c_str()))
			return FALSE;

		return TRUE;
	}

	void GLGraphics2D::Resize(INT cx, INT cy)
	{
		m_gl.Resize(cx, cy);
	}

	BOOL GLGraphics2D::BeginDraw()
	{
		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return TRUE;
	}
	BOOL GLGraphics2D::EndDraw()
	{
		if (m_gl.IsEmpty())
			return FALSE;
		return SwapBuffers(m_gl.GetDC());
	}
	BOOL GLGraphics2D::DrawImage(GLImage2D& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_gl))
			return FALSE;
		XMMATRIX* matrixs = m_gl.GetMatrixs();
		m_textureShader.SetShaderParameters(m_gl, matrixs[1], m_camera.GetView(), matrixs[2]);
		m_textureShader.Render(m_gl);
		return TRUE;
	}
	BOOL GLGraphics2D::DrawRectangle(GLRectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		if (!rectangle.DrawRectangle(m_gl, points, color))
			return FALSE;
		XMMATRIX* matrixs = m_gl.GetMatrixs();
		m_colorSharder.SetShaderParameters(m_gl, matrixs[1], m_camera.GetView(), matrixs[2]);
		m_colorSharder.Render(m_gl);
		return TRUE;
	}
	BOOL GLGraphics2D::FillRectangle(GLRectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		if (!rectangle.FillRectangle(m_gl, points, color))
			return FALSE;
		XMMATRIX* matrixs = m_gl.GetMatrixs();
		m_colorSharder.SetShaderParameters(m_gl, matrixs[1], m_camera.GetView(), matrixs[2]);
		m_colorSharder.Render(m_gl);
		return TRUE;
	}
}