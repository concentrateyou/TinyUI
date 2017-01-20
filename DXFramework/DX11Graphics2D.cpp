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
	BYTE* DX11Graphics2D::GetPointer(DWORD& dwSize)
	{
		m_dx11.Lock();
		TinyComPtr<ID3D11Resource> backBuffer;
		if (SUCCEEDED(m_dx11.GetSwap()->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
		{
			ID3D11Texture2D* texture2D = m_dx11.GetTexture2D();
			m_dx11.GetImmediateContext()->CopyResource(texture2D, backBuffer);
			D3D11_MAPPED_SUBRESOURCE ms = { 0 };
			if (SUCCEEDED(m_dx11.GetImmediateContext()->Map(texture2D, 0, D3D11_MAP_READ, 0, &ms)))
			{
				dwSize = ms.RowPitch * m_dx11.GetSize().cy;
				if (m_dwSize != dwSize)
				{
					m_dwSize = dwSize;
					m_bits.Reset(new BYTE[dwSize]);
				}
				memcpy(m_bits, static_cast<BYTE*>(ms.pData), dwSize);
				m_dx11.GetImmediateContext()->Unmap(texture2D, 0);
				m_dx11.Unlock();
				return m_bits;
			}
		}
		m_dx11.Unlock();
		return NULL;
	}
	TinySize DX11Graphics2D::GetSize() const
	{
		return m_dx11.GetSize();
	}
}
