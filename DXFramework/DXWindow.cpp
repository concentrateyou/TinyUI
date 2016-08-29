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
	void DXWindow::OnVideo(const BYTE* pBits, INT size, LPVOID lpParamer)
	{
		AM_MEDIA_TYPE* pMediaType = static_cast<AM_MEDIA_TYPE*>(lpParamer);
		if (pMediaType)
		{
			VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
			m_dx11.BeginScene();
			m_camera.UpdatePosition();
			D3DXMATRIX viewMatrix = m_camera.GetViewMatrix();
			D3DXMATRIX worldMatrix = m_dx11.GetWorldMatrix();
			D3DXMATRIX projectionMatrix = m_dx11.GetProjectionMatrix();
			D3DXMATRIX orthoMatrix = m_dx11.GetOrthoMatrix();
			m_dx11.AllowDepth(FALSE);
			m_dxVideo.FillImage(m_dx11, pBits, h->bmiHeader.biWidth, h->bmiHeader.biHeight);
			m_dxVideo.Render(m_dx11, 5, 5);
			m_textureShader.Render(m_dx11, m_dxVideo.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_dxVideo.GetTexture());
			m_dx11.AllowDepth(TRUE);
			m_dx11.EndScene();
		}
	}
	LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		vector<Media::VideoCapture::Name> names;
		Media::VideoCapture::GetDevices(names);
		vector<Media::VideoCaptureParam> params;
		Media::VideoCapture::GetDeviceParams(names[0], params);
		Media::VideoCaptureParam param;
		for (INT i = 0; i < params.size(); i++)
		{
			if (params[i].GetSize() == TinySize(640, 360))
			{
				param = params[i];
				break;
			}
		}
		m_videoCB = BindCallback(&DXWindow::OnVideo, this);
		m_videoCapture.Initialize(names[0], m_videoCB);
		m_videoCapture.Allocate(param);

		TinySize size(800, 600);
		CenterWindow(NULL, size);
		if (m_dx11.Initialize(m_hWND, 0, 0, 800, 600))
		{
			m_captureTask.Initialize(&m_dx11);
			m_camera.SetPosition(0.0F, 0.0F, -10.0F);
			if (m_textureShader.Initialize(m_dx11,
				TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.vs"),
				TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.ps")))
			{
				m_dxVideo.Create(m_dx11, 640, 360, 640, 360);
				m_videoCapture.Start();
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
		return FALSE;
	}
	LRESULT DXWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}
	BOOL DXWindow::Render()
	{
		m_dx11.BeginScene();
		m_camera.UpdatePosition();
		D3DXMATRIX viewMatrix = m_camera.GetViewMatrix();
		D3DXMATRIX worldMatrix = m_dx11.GetWorldMatrix();
		D3DXMATRIX projectionMatrix = m_dx11.GetProjectionMatrix();
		D3DXMATRIX orthoMatrix = m_dx11.GetOrthoMatrix();
		m_dx11.AllowDepth(FALSE);
		if (!m_dxVideo.Render(m_dx11, 1, 1))
			return FALSE;
		m_textureShader.Render(m_dx11, m_dxVideo.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_dxVideo.GetTexture());
		m_dx11.AllowDepth(TRUE);
		m_dx11.EndScene();
		return TRUE;
	}
}
