#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GL
	{
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		GL();
		~GL();
	public:
		BOOL	Initialize(HWND hWND, INT cx, INT cy);
		void	Uninitialize();
		BOOL	BeginDraw();
		BOOL	EndDraw();
	private:
		BOOL	GetPixelFormat(HDC hDC, INT& index, PIXELFORMATDESCRIPTOR& s);
	private:
		HDC		m_hDC;
		HWND	m_hWND;
		HGLRC	m_hGLRC;
	};
}



