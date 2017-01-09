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
		if (!m_dx11.Initialize(hWND, 0, 0, size.cx, size.cy))
			return FALSE;
		if (!m_textureShader.Initialize(m_dx11,
			TEXT("D:\\Develop\\TinyUI\\DXFramework\\texture.vs"),
			TEXT("D:\\Develop\\TinyUI\\DXFramework\\texture.ps")))
			return FALSE;
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		return TRUE;
	}

	BOOL DX11Graphics2D::BeginScene()
	{
		m_dx11.BeginScene();
		m_camera.UpdatePosition();
		m_viewMatrix = m_camera.GetViewMatrix();
		m_worldMatrix = m_dx11.GetWorldMatrix();
		m_orthoMatrix = m_dx11.GetOrthoMatrix();
		m_projectionMatrix = m_dx11.GetProjectionMatrix();
		return TRUE;
	}
	BOOL DX11Graphics2D::EndScene()
	{
		m_dx11.AllowDepth(TRUE);
		m_dx11.EndScene();
		return TRUE;
	}
	DX11& DX11Graphics2D::GetDX11()
	{
		return m_dx11;
	}
	BOOL DX11Graphics2D::DrawImage(DX11Image* pImage)
	{
		ASSERT(pImage);
		if (!pImage->IsValid())
			return FALSE;
		if (!pImage->Update(m_dx11))
			return FALSE;
		if (pImage->Render(m_dx11))
		{
			m_textureShader.Render(m_dx11, pImage->GetIndexCount(), m_worldMatrix, m_viewMatrix, m_orthoMatrix, pImage->GetTexture());
			return TRUE;
		}
		return FALSE;
	}
}
