#include "stdafx.h"
#include "GraphicsCapture.h"
#include <DXGIFormat.h>

GraphicsCapture::GraphicsCapture()
{
}


GraphicsCapture::~GraphicsCapture()
{
	WaitAll();
}

BOOL GraphicsCapture::CreateTexture(INT cx, INT cy)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = cx;
	desc.Height = cy;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	desc.BindFlags = 0;
	TinyComPtr<ID3D11Texture2D>	texture;
	if (FAILED(m_dx11.GetD3D()->CreateTexture2D(&desc, NULL, &texture)))
		return FALSE;
	if (FAILED(texture->QueryInterface(__uuidof(ID3D11Resource), (void**)&m_resource)))
		return FALSE;
	return TRUE;
}

BOOL GraphicsCapture::Initialize(HWND hWND, INT cx, INT cy)
{
	m_cx = cx;
	m_cy = cy;
	m_videoSize.cx = 640;
	m_videoSize.cy = 480;
	vector<Media::VideoCapture::Name> names;
	Media::VideoCapture::GetDevices(names);
	vector<Media::VideoCaptureParam> params;
	Media::VideoCapture::GetDeviceParams(names[0], params);
	Media::VideoCaptureParam param;
	for (UINT i = 0; i < params.size(); i++)
	{
		if (params[i].GetSize() == m_videoSize)
		{
			param = params[i];
			break;
		}
	}
	//m_publisher.Connect("rtmp://10.121.86.127/live/test");
	m_converter.Reset(new I420Converter(TinySize(m_cx, m_cy), TinySize(m_cx, m_cy)));
	m_x264Encode.Close();
	m_x264Encode.Open(m_cx, m_cy);
	if (!m_videoCapture.Initialize(names[0]))
		return FALSE;
	if (!m_videoCapture.Allocate(param))
		return FALSE;
	if (m_dx11.Initialize(hWND, 0, 0, m_cx, m_cy))
	{
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		if (m_textureShader.Initialize(m_dx11,
			TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.vs"),
			TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.ps")))
		{
			if (!CreateTexture(m_cx, m_cy))
				return FALSE;
			if (!m_videoImage.Create(m_dx11, m_videoSize.cx, m_videoSize.cy, m_videoSize.cx / 2, m_videoSize.cy / 2))
				return FALSE;
			if (!m_videoCapture.Start())
				return FALSE;
			m_publishTask.Reset(new PublishTask(this));
			m_publishTask->Submit();
			m_renderTask.Reset(new RenderTask(this));
			m_renderTask->Submit();
			m_captureTask.Reset(new DX11CaptureTask(&m_dx11, m_cx, m_cy));
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
	DX11Image* dxImage = m_captureTask->GetTexture();
	if (dxImage && dxImage->IsValid())
	{
		dxImage->Render(m_dx11, 1, 1);
		m_textureShader.Render(m_dx11, dxImage->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, dxImage->GetTexture());
	}
	if (m_videoCapture.GetPointer())
	{
		m_videoImage.FillImage(m_dx11, m_videoCapture.GetPointer(), m_videoSize.cx, m_videoSize.cy);
		m_videoImage.Render(m_dx11, m_cx - m_videoSize.cx / 2 - 1, m_cy - m_videoSize.cy / 2 - 1);
		m_textureShader.Render(m_dx11, m_videoImage.GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_videoImage.GetTexture());
	}
	m_dx11.AllowDepth(TRUE);
	m_dx11.EndScene();
	TinyComPtr<ID3D11Resource> backBuffer;
	if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
	{
		if (m_resource)
		{
			m_dx11.GetImmediateContext()->CopyResource(m_resource, backBuffer);
			D3D11_MAPPED_SUBRESOURCE ms = { 0 };
			if (SUCCEEDED(m_dx11.GetImmediateContext()->Map(m_resource, 0, D3D11_MAP_READ, 0, &ms)))
			{
				m_bits = static_cast<BYTE*>(ms.pData);
				m_dx11.GetImmediateContext()->Unmap(m_resource, 0);
			}
		}
	}
}

void GraphicsCapture::Publish()
{
	/*if (m_converter->BRGAToI420(m_bits))
	{
	m_x264Encode.Encode(m_converter->GetI420(), &m_publisher);
	}*/
}

void GraphicsCapture::WaitAll()
{
	m_publishTask->Wait(INFINITE);
	m_renderTask->Wait(INFINITE);
	m_captureTask->Wait(INFINITE);
}