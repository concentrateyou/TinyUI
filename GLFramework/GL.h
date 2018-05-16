#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GLRenderView;
	/// <summary>
	/// OpenGL 4.0
	/// </summary>
	class GL
	{
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		GL();
		virtual ~GL();
		BOOL		IsEmpty() const;
		BOOL		IsActive() const;
		HDC			GetDC() const;
		HWND		GetHWND() const;
		HGLRC		GetContext() const;
	public:
		BOOL		Initialize(HWND hWND, INT cx, INT cy);
		void		Uninitialize();
		void		SetMatrixs(const TinySize& size);
		XMMATRIX*	GetMatrixs();
	private:
		BOOL		LoadWGL();
	private:
		HDC					m_hDC;
		HWND				m_hWND;
		HGLRC				m_context;
		XMMATRIX			m_matrixs[3];
		GLRenderView*		m_render2D;
	};
}
