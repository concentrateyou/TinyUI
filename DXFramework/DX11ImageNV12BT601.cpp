#include "stdafx.h"
#include "DX11ImageNV12BT601.h"

namespace DXFramework
{
	DX11ImageNV12BT601::DX11ImageNV12BT601()
	{
	}

	DX11ImageNV12BT601::~DX11ImageNV12BT601()
	{
	}

	BOOL DX11ImageNV12BT601::Create(DX11& dx11, INT cx, INT cy)
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
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
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
	void DX11ImageNV12BT601::Destory()
	{
		m_textureY.Destory();
		m_textureNV.Destory();
	}
	BOOL DX11ImageNV12BT601::Copy(DX11& dx11, const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV)
	{
		D3D11_TEXTURE2D_DESC desc;
		m_textureY.GetTexture2D()->GetDesc(&desc);
		desc.Width = m_size.x;
		desc.Height = m_size.y;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_STAGING;
		DX11Texture2D staging;
		if (!staging.Create(dx11, desc))
			return FALSE;
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(ms));
		if (staging.Map(dx11, ms))
		{
			BYTE* dst = static_cast<BYTE*>(ms.pData);
			for (INT i = 0; i < desc.Height; i++)
			{
				memcpy(dst + ms.RowPitch*i, pY + strideY*i, strideY);
			}
			staging.Unmap(dx11);
			dx11.GetImmediateContext()->CopySubresourceRegion(m_textureY.GetTexture2D(), 0, desc.Width, desc.Height, 0, staging.GetTexture2D(), 0, NULL);
		}
		desc.Width = (m_size.x + 1) / 2;
		desc.Height = (m_size.y + 1) / 2;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_STAGING;
		staging.Destory();
		if (!staging.Create(dx11, desc))
			return FALSE;
		ZeroMemory(&ms, sizeof(ms));
		if (staging.Map(dx11, ms))
		{
			BYTE* dst = static_cast<BYTE*>(ms.pData);
			for (INT i = 0; i < desc.Height; i++)
			{
				memcpy(dst + ms.RowPitch*i, pUV + strideUV*i, strideUV);
			}
			staging.Unmap(dx11);
			dx11.GetImmediateContext()->CopySubresourceRegion(m_textureNV.GetTexture2D(), 0, desc.Width, desc.Height, 0, staging.GetTexture2D(), 0, NULL);
		}
		return TRUE;
	}
}

