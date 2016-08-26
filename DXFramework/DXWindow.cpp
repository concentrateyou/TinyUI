#include "stdafx.h"
#include "DXWindow.h"

namespace DXFramework
{
	DXWindow::DXWindow()
		:m_success(FALSE)
	{
	}
	DXWindow::~DXWindow()
	{
	}
	BOOL DXWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD DXWindow::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}
	DWORD DXWindow::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}
	LPCSTR DXWindow::RetrieveClassName()
	{
		return TEXT("IQiyiWindowClass");
	}
	LPCSTR DXWindow::RetrieveTitle()
	{
		return TEXT("DXWindow");
	}
	HICON DXWindow::RetrieveIcon()
	{
		return NULL;
	}
	LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinySize size(1200, 800);
		CenterWindow(NULL, size);
		if (m_dx10.Initialize(m_hWND, 0, 0, 1200, 800))
		{
			m_camera.SetPosition(0.0F, 0.0F, -10.0F);
			if (m_textureShader.Initialize(m_dx10, TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.fx")))
			{
				m_dxImage1.Load(m_dx10, TEXT("D:\\1.jpg"), 360, 225);
				m_dxImage2.Load(m_dx10, TEXT("D:\\2.jpg"), 512, 341);
			}
		}
		return FALSE;
	}
	LRESULT DXWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT DXWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}
	LRESULT DXWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		Render();
		return FALSE;
	}
	LRESULT DXWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}
	BOOL DXWindow::Render()
	{
		static FLOAT rotation = 0.0F;
		rotation += (FLOAT)D3DX_PI * 0.005F;
		if (rotation > 360.0F)
		{
			rotation -= 360.0F;
		}
		return Render(rotation);
	}
	BOOL DXWindow::Render(FLOAT rotation)
	{
		m_dx10.BeginScene();
		m_camera.UpdatePosition();
		D3DXMATRIX viewMatrix = m_camera.GetViewMatrix();
		D3DXMATRIX worldMatrix = m_dx10.GetWorldMatrix();
		D3DXMATRIX projectionMatrix = m_dx10.GetProjectionMatrix();
		D3DXMATRIX orthoMatrix = m_dx10.GetOrthoMatrix();
		m_dx10.AllowDepth(FALSE);
		if (!m_dxImage1.Render(m_dx10, 0, 200))
			return FALSE;
		m_textureShader.Render(m_dx10, m_dxImage1.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_dxImage1.GetTexture());
		if (!m_dxImage2.Render(m_dx10, 400, 200))
			return FALSE;
		m_textureShader.Render(m_dx10, m_dxImage2.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_dxImage2.GetTexture());
		m_dx10.AllowDepth(TRUE);
		m_dx10.EndScene();
		return TRUE;
	}
}
