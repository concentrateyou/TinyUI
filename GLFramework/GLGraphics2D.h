#pragma once
#include "GLCamera.h"
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
		BOOL	Initialize(HWND hWND, const TinySize& size);	
		GL&		GetGL();
		BOOL	BeginDraw();
		BOOL	EndDraw();
		BOOL	DrawImage(GLImage2D& image);
		BOOL	Present();
	private:
		GL				m_gl;
		GLCamera		m_camera;
		GLTextureShader	m_textureShader;
	};
}


