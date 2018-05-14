#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11Line2D.h"
#include "DX11Rectangle2D.h"
#include "DX11RenderView.h"
#include "DX11NV12BT601Shader.h"
#include "DX11ImageNV12BT601.h"

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
		BOOL			DrawImageNV12BT601(DX11ImageNV12BT601& image);
		BOOL			DrawImage(DX11Image2D& image);
		BOOL			DrawLine(DX11Line2D& line, XMFLOAT2* points, DWORD count, XMFLOAT4 color);
		BOOL			DrawRectangle(DX11Rectangle2D& rectangle, XMFLOAT2 points[4], UINT count, XMFLOAT4 color);
		BOOL			FillRectangle(DX11Rectangle2D& rectangle, XMFLOAT2 points[4], UINT count, XMFLOAT4 color);
	private:
		DX11					m_dx11;
		DXCamera				m_camera;
		DX11ColorShader			m_colorSharder;
		DX11TextureShader		m_textureShader;
		DX11NV12BT601Shader		m_shaderNV12BT601;
	};
}

