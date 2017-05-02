#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11ColorShader.h"
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
		BOOL	 Initialize(HWND hWND, const TinySize& size);
		void	 Present();
		BOOL     Resize(const TinySize& size);
		DX11&	 GetDX11();
		BOOL	 DrawImage(DX11Image2D* ps);
		BOOL	 DrawRectangle(DX11Rectangle2D* ps, const TinyRectangle& rectangle);
		void	 Lock();
		void	 Unlock();
		BYTE*	 GetPointer(DWORD& dwSize);
		TinySize GetSize() const;
	private:
		DX11					m_dx11;
		DXCamera				m_camera;
		DX11TextureShader		m_textureShader;
		DX11ColorShader			m_colorShader;
		DWORD					m_dwSize;
		TinyScopedPtr<BYTE>		m_bits;
	};
}

