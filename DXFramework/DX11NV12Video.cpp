#include "stdafx.h"
#include "DX11NV12Video.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11NV12Video, DX11ImageElement2D);

	DX11NV12Video::DX11NV12Video()
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 6));
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
		if (!Initialize(dx11))
			return FALSE;
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
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (!m_textureY.Create(dx11, desc))
			return FALSE;
		desc.Width = (cx + 1) / 2;
		desc.Height = (cy + 1) / 2;
		desc.Format = DXGI_FORMAT_R8G8_UNORM;
		if (!m_textureNV.Create(dx11, desc))
			return FALSE;
		SetSize(XMFLOAT2(static_cast<FLOAT>(cx), static_cast<FLOAT>(cy)));
		return TRUE;
	}
	void DX11NV12Video::Destory()
	{
		m_textureY.Destory();
		m_textureNV.Destory();
	}
	BOOL DX11NV12Video::DrawImage(DX11& dx11)
	{
		if (!Calculate(dx11))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexs, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(ms));
		if (!m_textureY.Map(dx11, ms))
			return FALSE;
		INT		cy = static_cast<INT>(m_size.y);
		BYTE*	dst = static_cast<BYTE*>(ms.pData);
		UINT	linesize = ms.RowPitch;
		for (INT i = 0; i < cy; i++)
		{
			memcpy(dst + i * linesize, pY + i * strideY, strideY);
		}
		m_textureY.Unmap(dx11);
		ZeroMemory(&ms, sizeof(ms));
		if (!m_textureNV.Map(dx11, ms))
			return FALSE;
		cy = static_cast<INT>((m_size.y + 1) / 2);
		dst = static_cast<BYTE*>(ms.pData);
		linesize = ms.RowPitch;
		for (INT i = 0; i < cy; i++)
		{
			memcpy(dst + i * linesize, pUV + i * strideUV, strideUV);
		}
		m_textureNV.Unmap(dx11);
		return TRUE;
	}
	BOOL DX11NV12Video::Initialize(DX11& dx11)
	{
		VERTEXTYPE vertexTypes[6];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 6));
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA	data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
		if (hRes != S_OK)
			return FALSE;
		ULONG indices[6];
		ZeroMemory(indices, (sizeof(ULONG) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexs);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11NV12Video::Calculate(DX11& dx11)
	{
		if (!m_vertex)
			return FALSE;
		m_vertexTypes[0].position = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[0].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[0].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[1].position = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[1].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[1].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[2].position = XMFLOAT3(-m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[2].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[2].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[3].position = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[3].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[3].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[4].position = XMFLOAT3(m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[4].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[4].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[5].position = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[5].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[5].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx11.GetImmediateContext()->RSGetViewports(&count, &vp);
		XMFLOAT2 size(m_size.x * m_scale.x, m_size.y * m_scale.y);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, 0.0F);
		XMMATRIX rotation = XMMatrixRotationZ(((m_angle) * (D3DX_PI / 180.0)));
		XMMATRIX translation = XMMatrixTranslation(-center.x + size.x / 2 + m_translate.x, center.y - size.y / 2 - m_translate.y, 0.0F);
		XMMATRIX val = scaling * rotation * translation;
		XMMATRIX* ms = dx11.GetMatrixs();
		ms[1] *= val;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 6);
		dx11.GetImmediateContext()->Unmap(m_vertex, 0);
		return TRUE;
	}
}

