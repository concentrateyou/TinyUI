#include "stdafx.h"
#include "DX11NV12Video.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11NV12Video, DX11ImageElement2D);

	DX11NV12Video::DX11NV12Video()
	{
	}

	DX11NV12Video::~DX11NV12Video()
	{
	}
	BOOL DX11NV12Video::IsEmpty() const
	{
		return m_textureY.IsEmpty() || m_textureNV.IsEmpty();
	}
	BOOL DX11NV12Video::Create(DX11& dx11, INT cx, INT cy)
	{
		Destory();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R8_UNORM;
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (!m_textureY.Create(dx11, desc))
			return FALSE;
		desc.Width = (cx + 1) / 2;
		desc.Height = (cy + 1) / 2;
		desc.Format = DXGI_FORMAT_R8G8_UNORM;
		if (!m_textureNV.Create(dx11, desc))
			return FALSE;
		m_size.x = static_cast<FLOAT>(cx);
		m_size.y = static_cast<FLOAT>(cy);
		return TRUE;
	}
	void DX11NV12Video::Destory()
	{
		m_textureY.Destory();
		m_textureNV.Destory();
	}
	BOOL DX11NV12Video::DrawImage(DX11& dx11)
	{
		m_textureY.DrawImage(dx11);
		m_textureNV.DrawImage(dx11);
		return TRUE;
	}
	DX11Texture2D*	DX11NV12Video::GetTextureY()
	{
		return &m_textureY;
	}
	DX11Texture2D*	DX11NV12Video::GetTextureNV()
	{
		return &m_textureNV;
	}
	BOOL DX11NV12Video::Copy(DX11& dx11, const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV)
	{
		D3D11_TEXTURE2D_DESC desc;
		m_textureY.GetTexture2D()->GetDesc(&desc);
		if (m_stagingY.IsEmpty())
		{
			desc.Width = m_size.x;
			desc.Height = m_size.y;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_STAGING;
			if (!m_stagingY.Create(dx11, desc))
				return FALSE;
		}
		if (m_stagingNV.IsEmpty())
		{
			desc.Width = (m_size.x + 1) / 2;
			desc.Height = (m_size.y + 1) / 2;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_STAGING;
			m_stagingNV.Destory();
			if (!m_stagingNV.Create(dx11, desc))
				return FALSE;
		}
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(ms));
		if (m_stagingY.Map(dx11, ms))
		{
			BYTE* dst = static_cast<BYTE*>(ms.pData);
			INT x = static_cast<INT>(m_size.x);
			INT y = static_cast<INT>(m_size.y);
			for (INT i = 0; i < y; i++)
			{
				memcpy(dst + ms.RowPitch*i, pY + strideY*i, strideY);
			}
			m_stagingY.Unmap(dx11);
			dx11.GetImmediateContext()->CopySubresourceRegion(m_textureY.GetTexture2D(), 0, x, y, 0, m_stagingY.GetTexture2D(), 0, NULL);
		}
		ZeroMemory(&ms, sizeof(ms));
		if (m_stagingNV.Map(dx11, ms))
		{
			BYTE* dst = static_cast<BYTE*>(ms.pData);
			INT x = static_cast<INT>((m_size.x + 1) / 2);
			INT y = static_cast<INT>((m_size.y + 1) / 2);
			for (INT i = 0; i < y; i++)
			{
				memcpy(dst + ms.RowPitch*i, pUV + strideUV*i, strideUV);
			}
			m_stagingNV.Unmap(dx11);
			dx11.GetImmediateContext()->CopySubresourceRegion(m_textureNV.GetTexture2D(), 0, x, y, 0, m_stagingNV.GetTexture2D(), 0, NULL);
		}
		return TRUE;
	}
}

