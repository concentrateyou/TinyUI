#include "stdafx.h"
#include "DXSystem.h"


DXSystem::DXSystem()
{
}


DXSystem::~DXSystem()
{
}

BOOL DXSystem::Initialize(HWND hWND, INT cx, INT cy)
{
	vector<Media::VideoCapture::Name> names;
	Media::VideoCapture::GetDevices(names);
	vector<Media::VideoCaptureParam> params;
	Media::VideoCapture::GetDeviceParams(names[0], params);
	Media::VideoCaptureParam param;
	for (UINT i = 0; i < params.size(); i++)
	{
		if (params[i].GetSize() == TinySize(640, 360))
		{
			param = params[i];
			break;
		}
	}
	if (!m_videoCapture.Initialize(names[0]))
		return FALSE;
	if (!m_videoCapture.Allocate(param))
		return FALSE;
	if (!m_tasks.Initialize(0, 10))
		return FALSE;
	if (m_dx11.Initialize(hWND, 0, 0, cx, cy))
	{
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		if (m_textureShader.Initialize(m_dx11,
			TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.vs"),
			TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.ps")))
		{
			if (!m_dxVideo.Create(m_dx11, 640, 360, 320, 180))
				return FALSE;
			if (!m_videoCapture.Start())
				return FALSE;
			m_renderTask.Reset(new DXRenderTask(this, &m_tasks));
			m_renderTask->Submit();
			m_captureTask.Reset(new DX11CaptureTask(&m_dx11, &m_tasks));
			m_captureTask->Submit();
		}
	}
	return TRUE;
}

void DXSystem::Render()
{
	m_dx11.BeginScene();
	m_camera.UpdatePosition();
	D3DXMATRIX viewMatrix = m_camera.GetViewMatrix();
	D3DXMATRIX worldMatrix = m_dx11.GetWorldMatrix();
	D3DXMATRIX projectionMatrix = m_dx11.GetProjectionMatrix();
	D3DXMATRIX orthoMatrix = m_dx11.GetOrthoMatrix();
	m_dx11.AllowDepth(FALSE);
	BYTE* pBits = m_videoCapture.GetData();
	if (pBits)
	{
		m_dxVideo.FillImage(m_dx11, pBits, 640, 360);
		m_dxVideo.Render(m_dx11, 5, 5);
		m_textureShader.Render(m_dx11, m_dxVideo.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_dxVideo.GetTexture());
	}
	DX11Image* dxImage = m_captureTask->GetTexture();
	if (dxImage && dxImage->IsValid())
	{
		dxImage->Render(m_dx11, 150, 150);
		m_textureShader.Render(m_dx11, dxImage->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, dxImage->GetTexture());
	}
	m_dx11.AllowDepth(TRUE);
	m_dx11.EndScene();
}

void DXSystem::Cancel()
{
	if (m_renderTask)
		m_renderTask->Cancel();
}
