#pragma once
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
		BOOL	Initialize(HWND hWND, const TinySize& size);	
	private:
		GLCamera		m_camera;
		GLTextureShader	m_textureShader;
	};
}


