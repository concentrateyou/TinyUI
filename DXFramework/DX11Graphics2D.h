#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11Rectangle2D.h"
#include "DX11RenderView.h"

namespace DXFramework
{
	/// <summary>
	/// DX11»­²¼
	/// </summary>
	class DX11Graphics2D
	{
	public:
		DX11Graphics2D();
		virtual ~DX11Graphics2D();
		BOOL	Initialize(HWND hWND, const TinySize& size);
		void	Present();
		void	Flush();
		BOOL    Resize(const TinySize& size);
		DX11&	GetDX11();
		BOOL	DrawImage(DX11Image2D* ps, FLOAT ratioX = 1.0F, FLOAT ratioY = 1.0F);
		void	Enter();
		void	Leave();
		void	SetRenderView(DX11RenderView* render2D);
		DX11RenderView*		GetRenderView() const;
	private:
		DX11					m_dx11;
		DXCamera				m_camera;
		DX11TextureShader		m_textureShader;
	};
}

