#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class OpenGL
	{
		DISALLOW_COPY_AND_ASSIGN(OpenGL)
	public:
		OpenGL();
		~OpenGL();
	public:
		BOOL	Initialize(HWND hWND, INT cx, INT cy);
		void	Uninitialize();
		BOOL	BeginDraw();
		BOOL	EndDraw();
	private:
		BOOL	GetPixelFormat(INT& index, PIXELFORMATDESCRIPTOR& s);
	private:
		HDC		m_hDC;
		HWND	m_hWND;
		HGLRC	m_hGLRC;
	};
}



