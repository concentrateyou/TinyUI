#include "stdafx.h"
#include "DX11Graphics2D.h"

namespace DXFramework
{
	DX11Graphics2D::DX11Graphics2D()
		:m_dwSize(0)
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
		m_resource.Release();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = size.cx;
		desc.Height = size.cy;
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
	BOOL DX11Graphics2D::Resize(const TinySize& size)
	{
		return m_dx11.ResizeView(size.cx, size.cy);
	}
	DX11& DX11Graphics2D::GetDX11()
	{
		return m_dx11;
	}
	BOOL DX11Graphics2D::DrawImage(DX11Image* pImage)
	{
		ASSERT(pImage);
		if (!pImage->IsEmpty())
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
	void DX11Graphics2D::Lock()
	{
		m_dx11.Lock();
	}
	void DX11Graphics2D::Unlock()
	{
		m_dx11.Unlock();
	}
	BOOL DX11Graphics2D::GetPointer(BYTE* bits, DWORD& dwSize)
	{
		m_dx11.Lock();
		TinyComPtr<ID3D11Resource> backBuffer;
		if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
		{
			m_dx11.GetImmediateContext()->CopyResource(m_resource, backBuffer);
			D3D11_MAPPED_SUBRESOURCE ms = { 0 };
			if (SUCCEEDED(m_dx11.GetImmediateContext()->Map(m_resource, 0, D3D11_MAP_READ, 0, &ms)))
			{
				dwSize = ms.RowPitch * m_dx11.GetSize().cy;
				if (m_dwSize != dwSize)
				{
					m_dwSize = dwSize;
					m_bits.Reset(new BYTE[dwSize]);
				}
				memcpy(m_bits, static_cast<BYTE*>(ms.pData), dwSize);
				m_dx11.GetImmediateContext()->Unmap(m_resource, 0);
				m_dx11.Unlock();
				return TRUE;
			}
		}
		m_dx11.Unlock();
		return FALSE;
	}
	TinySize DX11Graphics2D::GetSize() const
	{
		return m_dx11.GetSize();
	}
}
