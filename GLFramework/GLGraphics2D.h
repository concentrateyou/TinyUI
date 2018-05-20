#pragma once
#include "GLImage2D.h"
#include "GLCamera.h"
#include "GLTextureShader.h"

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
		BOOL	Initialize(HWND hWND, INT cx, INT cy);
		void	Resize(INT cx, INT cy);
		BOOL	BeginDraw();
		BOOL	EndDraw();
		BOOL	DrawImage(GLImage2D& image);
	private:
		GL					m_gl;
		GLCamera			m_camera;
		GLTextureShader		m_textureShader;
	};
}