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
		return m_dx11.ResizeView(size.cx, size.cy);
	}

	DX11& DX11Graphics2D::GetDX11()
	{
		return m_dx11;
	}

	BOOL DX11Graphics2D::DrawImage(DX11Image2D* ps, FLOAT ratioX, FLOAT ratioY)
	{
		ASSERT(ps);
		if (ps->IsEmpty())
			return FALSE;
		if (!m_dx11.GetRender2D())
			return FALSE;
		if (!ps->Transform(m_dx11, ratioX, ratioY))
			return FALSE;
		if (ps->Process(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_textureShader.Render(m_dx11, ps->GetIndexCount(), ms[1], m_camera.GetView(), ms[2], ps->GetTexture2D());
			return TRUE;
		}
		return FALSE;
	}

	void DX11Graphics2D::Enter()
	{
		m_dx11.Lock();
	}

	void DX11Graphics2D::Leave()
	{
		m_dx11.Unlock();
	}
}
