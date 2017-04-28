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
		if (!m_dx11.Initialize(hWND, size.cx, size.cy))
			return FALSE;
		string str;
		str.resize(MAX_PATH);
		GetModuleFileName(NULL, &str[0], MAX_PATH);
		str = str.substr(0, str.find_last_of("\\", string::npos, 1));
		string vs = str + "\\texture.vs";
		ASSERT(PathFileExists(vs.c_str()));
		string ps = str + "\\texture.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_textureShader.Initialize(m_dx11, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\color.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\color.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_colorShader.Initialize(m_dx11, vs.c_str(), ps.c_str()))
			return FALSE;
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		return TRUE;
	}

	void DX11Graphics2D::BeginDraw()
	{
		m_dx11.BeginDraw();
		m_camera.UpdatePosition();
	}
	void DX11Graphics2D::EndDraw()
	{
		m_dx11.EndDraw();
	}
	BOOL DX11Graphics2D::Resize(const TinySize& size)
	{
		return m_dx11.ResizeView(size.cx, size.cy);
	}
	DX11& DX11Graphics2D::GetDX11()
	{
		return m_dx11;
	}
	BOOL DX11Graphics2D::DrawImage(DX11Image2D* ps)
	{
		ASSERT(ps);
		if (!ps->IsEmpty())
			return FALSE;
		if (!ps->Update(m_dx11))
			return FALSE;
		if (ps->Render(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_textureShader.Render(m_dx11, ps->GetIndexCount(), ms[1], m_camera.GetView(), ms[2], ps->GetTexture());
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Graphics2D::DrawRectangle(DX11Rectangle2D* ps, const TinyRectangle& rectangle)
	{
		ASSERT(ps);
		if (!ps->SetRectangle(m_dx11, rectangle))
			return FALSE;
		if (ps->Render(m_dx11))
		{
			XMMATRIX* ms = m_dx11.GetMatrixs();
			m_colorShader.Render(m_dx11, ps->GetIndexCount(), ms[1], m_camera.GetView(), ms[2]);
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
