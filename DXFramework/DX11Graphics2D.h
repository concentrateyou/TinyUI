#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
#include "DXCamera.h"
#include "DX11TextureShader.h"

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
		BOOL	 BeginScene();
		BOOL	 EndScene();
		BOOL     Resize(const TinySize& size);
		DX11&	 GetDX11();
		BOOL	 DrawImage(DX11Image* pImage);
		void	 Lock();
		void	 Unlock();
		BOOL	GetPointer(BYTE* bits, DWORD& dwSize);
		TinySize GetSize() const;
	private:
		DWORD						m_dwSize;
		TinyScopedPtr<BYTE>			m_bits;
		DX11						m_dx11;
		DXCamera					m_camera;
		D3DXMATRIX					m_viewMatrix;
		D3DXMATRIX					m_worldMatrix;
		D3DXMATRIX					m_projectionMatrix;
		D3DXMATRIX					m_orthoMatrix;
		DX11TextureShader			m_textureShader;
		TinyComPtr<ID3D11Resource>	m_resource;
	};
}

