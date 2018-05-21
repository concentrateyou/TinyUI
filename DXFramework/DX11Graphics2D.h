#pragma once
#include "DXFramework.h"
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11Line2D.h"
#include "DX11Rectangle2D.h"
#include "DX11RenderView.h"
#include "DX11NV12BT601Shader.h"
#include "DX11NV12BT709Shader.h"
#include "DX11NV12Video.h"

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
		BOOL			Initialize(HWND hWND, const TinySize& size);
		void			Present();
		void			Flush();
		BOOL			Resize(const TinySize& size);
		DX11&			GetDX11();
		void			SetRenderView(DX11RenderView* render2D);
		DX11RenderView*	GetRenderView() const;
	public:
		BOOL			DrawImageNV12BT601(DX11NV12Video& image);
		BOOL			DrawImageNV12BT709(DX11NV12Video& image);
		BOOL			DrawImage(DX11Image2D& image);
		BOOL			DrawLine(DX11Line2D& line,const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
		BOOL			DrawRectangle(DX11Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL			FillRectangle(DX11Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
	private:
		DX11					m_dx11;
		DXCamera				m_camera;
		DX11ColorShader			m_colorSharder;
		DX11TextureShader		m_textureShader;
		DX11NV12BT601Shader		m_shaderNV12BT601;
		DX11NV12BT709Shader		m_shaderNV12BT709;
	};
}

