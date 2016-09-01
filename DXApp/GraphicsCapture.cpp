#include "stdafx.h"
#include "GraphicsCapture.h"
#include <DXGIFormat.h>


GraphicsCapture::GraphicsCapture()
{
}


GraphicsCapture::~GraphicsCapture()
{
}

BOOL GraphicsCapture::CreatePublishTexture(INT cx, INT cy)
{
	DXGI_SWAP_CHAIN_DESC scd;
	if (FAILED(m_dx11.GetSwap()->GetDesc(&scd)))
		return FALSE;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = cx;
	desc.Height = cy;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = scd.BufferDesc.Format;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
	TinyComPtr<ID3D11Texture2D>	texture;
	if (FAILED(m_dx11.GetD3D()->CreateTexture2D(&desc, NULL, &texture)))
		return FALSE;
	if (FAILED(texture->QueryInterface(__uuidof(ID3D11Resource), (void**)&m_publishRes)))
		return FALSE;
	return TRUE;
}

BOOL GraphicsCapture::Initialize(HWND hWND, INT cx, INT cy)
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
			if (!CreatePublishTexture(cx, cy))
				return FALSE;
			m_publishTask.Reset(new PublishTask(this, &m_tasks));
			m_publishTask->Submit();
			m_renderTask.Reset(new RenderTask(this, &m_tasks));
			m_renderTask->Submit();
			m_captureTask.Reset(new DX11CaptureTask(&m_dx11, &m_tasks));
			m_captureTask->Submit();
		}
	}
	return TRUE;
}

void GraphicsCapture::Render()
{
	m_dx11.BeginScene();
	m_camera.UpdatePosition();
	D3DXMATRIX viewMatrix = m_camera.GetViewMatrix();
	D3DXMATRIX worldMatrix = m_dx11.GetWorldMatrix();
	D3DXMATRIX projectionMatrix = m_dx11.GetProjectionMatrix();
	D3DXMATRIX orthoMatrix = m_dx11.GetOrthoMatrix();
	m_dx11.AllowDepth(FALSE);
	BYTE* pBits = m_videoCapture.GetPointer();
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

void GraphicsCapture::Publish()
{
	TinyComPtr<ID3D11Resource> backBuffer;
	if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
	{
		m_dx11.GetContext()->CopyResource(m_publishRes, backBuffer);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(m_dx11.GetContext()->Map(m_publishRes, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			m_dx11.GetContext()->Unmap(m_publishRes, 0);
		}
	}
}