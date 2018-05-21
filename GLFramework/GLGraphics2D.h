#pragma once
#include "GLImage2D.h"
#include "GLRectangle2D.h"
#include "GLLine2D.h"
#include "GLCamera.h"
#include "GLColorShader.h"
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
	public:
		BOOL	DrawImage(GLImage2D& image);
		BOOL	DrawRectangle(GLRectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL	FillRectangle(GLRectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL	DrawLine(GLLine2D& line, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
	private:
		GL					m_gl;
		GLCamera			m_camera;
		GLColorShader		m_colorSharder;
		GLTextureShader		m_textureShader;
	};
}