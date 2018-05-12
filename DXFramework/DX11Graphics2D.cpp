#include "stdafx.h"
#include "DX11Graphics2D.h"

namespace DXFramework
{
	DX11Graphics2D::DX11Graphics2D()
	{
	}

	DX11Graphics2D::~DX11Graphics2D()
	{
	}

	BOOL DX11Graphics2D::Initialize(HWND hWND, const TinySize& size)
	{
		if (!m_dx11.Initialize(hWND, size.cx, size.cy, NULL))
			return FALSE;
		string str;
		str.resize(MAX_PATH);
		GetModuleFileName(NULL, &str[0], MAX_PATH);
		str = str.substr(0, str.find_last_of("\\", string::npos, 1));
		string vs = str + "\\texture.vs";
		ASSERT(PathFileExists(vs.c_str()));
		string ps = str + "\\texture.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_textureShader.Initialize(m_dx11, vs.c_str(), ps.c_str()))
			return FALSE;

		vs = str + "\\color.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\color.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_colorSharder.Initialize(m_dx11, vs.c_str(), ps.c_str()))
			return FALSE;

		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		m_camera.UpdatePosition();
		return TRUE;
	}

	void DX11Graphics2D::Present()
	{
		m_dx11.Present();
	}

	void DX11Graphics2D::Flush()
	{
		m_dx11.Flush();
	}

	BOOL DX11Graphics2D::Resize(const TinySize& size)
	{
		return m_dx11.Resize(size.cx, size.cy);
	}

	DX11& DX11Graphics2D::GetDX11()
	{
		return m_dx11;
	}

	BOOL DX11Graphics2D::DrawImage(DX11Image2D& image)
	{
		if (!m_dx11.GetRenderView())
			return FALSE;
		if (image.IsEmpty())
			return FALSE;
		if (image.Process(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_textureShader.SetShaderParameters(m_dx11, ms[1], m_camera.GetView(), ms[2], &image);
			m_textureShader.Render(m_dx11);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Graphics2D::DrawLine(DX11Line2D& line)
	{
		if (!m_dx11.GetRenderView())
			return FALSE;
		if (line.Process(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_colorSharder.SetShaderParameters(m_dx11, ms[1], m_camera.GetView(), ms[2], line.GetIndexs());
			m_colorSharder.Render(m_dx11);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Graphics2D::DrawRectangle(DX11Rectangle2D& rectangle)
	{
		if (!m_dx11.GetRenderView())
			return FALSE;
		rectangle.SetPrimitiveTopology(m_dx11, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		if (rectangle.Process(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_colorSharder.SetShaderParameters(m_dx11, ms[1], m_camera.GetView(), ms[2], rectangle.GetIndexs());
			m_colorSharder.Render(m_dx11);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Graphics2D::FillRectangle(DX11Rectangle2D& rectangle)
	{
		if (!m_dx11.GetRenderView())
			return FALSE;
		rectangle.SetPrimitiveTopology(m_dx11, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		if (rectangle.Process(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_colorSharder.SetShaderParameters(m_dx11, ms[1], m_camera.GetView(), ms[2], rectangle.GetIndexs());
			m_colorSharder.Render(m_dx11);
			return TRUE;
		}
		return FALSE;
	}
	void DX11Graphics2D::SetRenderView(DX11RenderView* render2D)
	{
		m_dx11.SetRenderView(render2D);
	}
	DX11RenderView*	DX11Graphics2D::GetRenderView() const
	{
		return m_dx11.GetRenderView();
	}
}
