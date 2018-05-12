#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11Line2D.h"
#include "DX11Rectangle2D.h"
#include "DX11RenderView.h"

namespace DXFramework
{
	/// <summary>
	/// DX11����
	/// </summary>
	class DX11Graphics2D
	{
	public:
		DX11Graphics2D();
		virtual ~DX11Graphics2D();
		BOOL			Initialize(HWND hWND, const TinySize& size);
		void			Present();
		void			Flush();
		BOOL			Resize(const TinySize& size);
		DX11&			GetDX11();
		void			SetRenderView(DX11RenderView* render2D);
		DX11RenderView*	GetRenderView() const;
	public:
		BOOL			DrawImage(DX11Image2D& image);
		BOOL			DrawLine(DX11Line2D& line);
		BOOL			DrawRectangle(DX11Rectangle2D& rectangle);
		BOOL			FillRectangle(DX11Rectangle2D& rectangle);
		BOOL			DrawString();
	private:
		DX11					m_dx11;
		DXCamera				m_camera;
		DX11ColorShader			m_colorSharder;
		DX11TextureShader		m_textureShader;
	};
}

