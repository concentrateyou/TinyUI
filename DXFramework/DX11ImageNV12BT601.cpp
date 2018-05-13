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
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		if (!m_textureY.Create(dx11, desc))
			return FALSE;
		desc.Width = (cx + 1) / 2;
		desc.Height = (cy + 1) / 2;
		desc.Format = DXGI_FORMAT_R8G8_UNORM;
		if (!m_textureNV.Create(dx11, desc))
			return FALSE;
		return TRUE;
	}
	void DX11ImageNV12BT601::Destory()
	{
		m_textureY.Destory();
		m_textureNV.Destory();
	}
	BOOL DX11ImageNV12BT601::Copy(const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV)
	{

		return TRUE;
	}
}

