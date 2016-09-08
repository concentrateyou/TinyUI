#include "stdafx.h"
#include "DXGraphics.h"
#include <DXGIFormat.h>

DXGraphics::DXGraphics()
{
}


DXGraphics::~DXGraphics()
{

}

BOOL DXGraphics::CreateTexture(INT cx, INT cy)
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

BOOL DXGraphics::Initialize(HWND hWND, INT cx, INT cy)
{
	if (!m_dx11.Initialize(hWND, 0, 0, cx, cy))
		return FALSE;
	m_camera.SetPosition(0.0F, 0.0F, -10.0F);
	if (!m_textureShader.Initialize(m_dx11,
		TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.vs"),
		TEXT("D:\\Develop\\GitHub\\TinyUI\\DXFramework\\texture.ps")))
		return FALSE;
	if (!CreateTexture(cx, cy))
		return FALSE;
	return TRUE;
}

BOOL DXGraphics::BeginScene()
{
	m_dx11.BeginScene();
	m_camera.UpdatePosition();
}
BOOL DXGraphics::EndScene()
{
	m_dx11.AllowDepth(TRUE);
	m_dx11.EndScene();
	TinyComPtr<ID3D11Resource> backBuffer;
	if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer))
		&& m_resource)
	{
		m_dx11.GetImmediateContext()->CopyResource(m_resource, backBuffer);
		D3D11_MAPPED_SUBRESOURCE ms = { 0 };
		if (SUCCEEDED(m_dx11.GetImmediateContext()->Map(m_resource, 0, D3D11_MAP_READ, 0, &ms)))
		{
			m_bits = static_cast<BYTE*>(ms.pData);
			m_dx11.GetImmediateContext()->Unmap(m_resource, 0);
			return TRUE;
		}
	}
	return FALSE;
}

BYTE* DXGraphics::GetPointer() const
{
	return m_bits;
}
