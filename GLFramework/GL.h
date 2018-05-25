#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GL
	{
		friend class GLGraphics2D;
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		GL();
		~GL();
	public:
		BOOL		IsEmpty() const;
		BOOL		IsActive() const;
		HWND		GetHWND() const;
		glm::vec2	GetSize() const;
		glm::mat4*	GetMatrixs();
	public:
		BOOL		Initialize(HWND hWND, INT cx, INT cy);
		void		Uninitialize();
		void		Resize(INT cx, INT cy);
	private:
		BOOL		GetPixelFormat(HDC hDC, INT& index, PIXELFORMATDESCRIPTOR& s);
		BOOL		BeginDraw();
		BOOL		EndDraw();
	private:
		HDC			m_hDC;
		HWND		m_hWND;
		HGLRC		m_hGLRC;
		glm::vec2	m_size;
		glm::mat4	m_matrixs[3];
	};
}



