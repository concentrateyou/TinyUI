#pragma once
#include "GLCamera.h"
#include "GL.h"
#include "GLTextureShader.h"
#include "GLImage2D.h"

namespace GLFramework
{
	class GLGraphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(GLGraphics2D)
	public:
		GLGraphics2D();
		~GLGraphics2D();
	public:
		GL&		GetGL();
	public:
		BOOL	Initialize(HWND hWND, const TinySize& size);	
		BOOL	BeginDraw();
		BOOL	EndDraw();
		BOOL	DrawImage(GLImage2D& image);
	private:
		GL				m_gl;
		GLCamera		m_camera;
		GLTextureShader	m_textureShader;
	};
}


