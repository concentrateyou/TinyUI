#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11ColorShader.h"
#include "DX11Rectangle2D.h"

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
		BOOL	 BeginDraw();
		BOOL	 EndDraw();
		BOOL     Resize(const TinySize& size);
		DX11&	 GetDX11();
		BOOL	 DrawImage(DX11Image2D* ps);
		BOOL	 DrawRectangle(DX11Rectangle2D* ps, const TinyRectangle& rectangle);
		void	 Lock();
		void	 Unlock();
		BYTE*	 GetPointer(DWORD& dwSize);
		TinySize GetSize() const;
	private:
		DWORD					m_dwSize;
		TinyScopedPtr<BYTE>		m_bits;
		DX11					m_dx11;
		DXCamera				m_camera;
		XMMATRIX				m_viewMatrix;
		XMMATRIX				m_worldMatrix;
		XMMATRIX				m_projectionMatrix;
		XMMATRIX				m_orthoMatrix;
		DX11TextureShader		m_textureShader;
		DX11ColorShader			m_colorShader;
	};
}

