#include "stdafx.h"
#include "GraphicsCapture.h"
#include <DXGIFormat.h>

GraphicsCapture::GraphicsCapture()
	:m_dwSize(0)
{
}


GraphicsCapture::~GraphicsCapture()
{

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
	m_converter.Reset(new I420Converter(cx, cy));
	m_cx = cx;
	m_cy = cy;
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
			if (!CreateTexture(cx, cy))
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

BOOL GraphicsCapture::Resize(INT cx, INT cy)
{
	if (!m_dx11.TryLock())
		return FALSE;
	m_cx = cx;
	m_cy = cy;
	BOOL bRes = m_dx11.ResizeView(cx, cy);
	m_dx11.Unlock();
	return bRes;
}

void GraphicsCapture::Render()
{
	if (!m_dx11.TryLock())
		return;
	//绘制纹理
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
	//拷贝屏幕纹理数据
	TinyComPtr<ID3D11Resource> backBuffer;
	if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
	{
		m_dx11.GetContext()->CopyResource(m_resource, backBuffer);
		D3D11_MAPPED_SUBRESOURCE ms = { 0 };
		if (SUCCEEDED(m_dx11.GetContext()->Map(m_resource, 0, D3D11_MAP_READ, 0, &ms)))
		{
			DWORD dwSize = m_cy * ms.RowPitch;
			if (dwSize != m_dwSize)
			{
				m_x264Encode.Close();
				m_x264Encode.Open(m_cx, m_cy);
				m_dwSize = dwSize;
				m_bits.Reset(new BYTE[dwSize]);
			}
			if (m_bits)
			{
				memcpy(m_bits, ms.pData, m_dwSize);
			}
			m_dx11.GetContext()->Unmap(m_resource, 0);
		}
	}
	m_dx11.Unlock();
}

void GraphicsCapture::Publish()
{
	if (m_bits)
	{
		m_converter->BRGAToI420(m_bits);
		m_x264Encode.Encode(m_converter->GetI420());
	}
}

BYTE* GraphicsCapture::GetPointer() const
{
	return m_bits;
}
DWORD GraphicsCapture::GetSize() const
{
	return m_dwSize;
}