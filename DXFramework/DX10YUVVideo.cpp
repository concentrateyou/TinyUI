#include "stdafx.h"
#include "DX10YUVVideo.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10YUVVideo, DX10Texture2DElement);

	DX10YUVVideo::DX10YUVVideo()
	{
	}

	DX10YUVVideo::~DX10YUVVideo()
	{
	}
	BOOL DX10YUVVideo::IsEmpty() const
	{
		return m_textureY.IsEmpty() || m_textureU.IsEmpty() || m_textureV.IsEmpty();
	}
	BOOL DX10YUVVideo::Create(DX10& dx10, INT cx, INT cy)
	{
		if (!Initialize(dx10))
			return FALSE;
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R8_UNORM;
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		if (!m_textureY.Create(dx10, desc))
			return FALSE;
		desc.Width = (cx + 1) / 2;
		desc.Height = (cy + 1) / 2;
		desc.Format = DXGI_FORMAT_R8_UNORM;
		if (!m_textureU.Create(dx10, desc))
			return FALSE;
		if (!m_textureV.Create(dx10, desc))
			return FALSE;
		SetSize(XMFLOAT2(static_cast<FLOAT>(cx), static_cast<FLOAT>(cy)));
		return TRUE;
	}
	void DX10YUVVideo::Destory()
	{
		m_textureY.Destory();
		m_textureU.Destory();
		m_textureV.Destory();
	}
	BOOL DX10YUVVideo::DrawImage(DX10& dx10)
	{
		if (!Calculate(dx10))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_index, DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
	BOOL DX10YUVVideo::Copy(DX10& dx10, const BYTE* pY, UINT strideY, const BYTE* pU, UINT strideU, const BYTE* pV, UINT strideV)
	{
		//Y
		D3D10_MAPPED_TEXTURE2D ms;
		ZeroMemory(&ms, sizeof(ms));
		if (!m_textureY.Map(ms))
			return FALSE;
		INT		cy = static_cast<INT>(m_size.y);
		BYTE*	dst = static_cast<BYTE*>(ms.pData);
		UINT	linesize = ms.RowPitch;
		for (INT i = 0; i < cy; i++)
		{
			memcpy(dst + i * linesize, pY + i * strideY, strideY);
		}
		m_textureY.Unmap();
		//U
		ZeroMemory(&ms, sizeof(ms));
		if (!m_textureU.Map(ms))
			return FALSE;
		cy = static_cast<INT>((m_size.y + 1) / 2);
		dst = static_cast<BYTE*>(ms.pData);
		linesize = ms.RowPitch;
		for (INT i = 0; i < cy; i++)
		{
			memcpy(dst + i * linesize, pU + i * strideU, strideU);
		}
		m_textureU.Unmap();
		//V
		ZeroMemory(&ms, sizeof(ms));
		if (!m_textureV.Map(ms))
			return FALSE;
		cy = static_cast<INT>((m_size.y + 1) / 2);
		dst = static_cast<BYTE*>(ms.pData);
		linesize = ms.RowPitch;
		for (INT i = 0; i < cy; i++)
		{
			memcpy(dst + i * linesize, pV + i * strideV, strideV);
		}
		m_textureV.Unmap();
		return TRUE;
	}
	DX10Texture2D*	DX10YUVVideo::GetTextureY()
	{
		return &m_textureY;
	}
	DX10Texture2D*	DX10YUVVideo::GetTextureU()
	{
		return &m_textureU;
	}
	DX10Texture2D*	DX10YUVVideo::GetTextureV()
	{
		return &m_textureV;
	}
	BOOL DX10YUVVideo::Initialize(DX10& dx10)
	{
		VERTEXTYPE vertexTypes[6];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 6));
		D3D10_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA	data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
		if (hRes != S_OK)
			return FALSE;
		ULONG indices[6];
		ZeroMemory(indices, (sizeof(ULONG) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_index);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX10YUVVideo::Calculate(DX10& dx10)
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
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx10.GetD3D()->RSGetViewports(&count, &vp);
		XMFLOAT2 size(m_size.x * m_scale.x, m_size.y * m_scale.y);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, 0.0F);
		XMMATRIX rotation = XMMatrixRotationZ(((m_angle) * (D3DX_PI / 180.0)));
		XMMATRIX translation = XMMatrixTranslation(-center.x + size.x / 2 + m_translate.x, center.y - size.y / 2 - m_translate.y, 0.0F);
		XMMATRIX val = scaling * rotation * translation;
		XMMATRIX* ms = dx10.GetMatrixs();
		ms[1] *= val;
		BYTE* bits = NULL;
		HRESULT hRes = m_vertex->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 6);
		m_vertex->Unmap();
		return TRUE;
	}
}

