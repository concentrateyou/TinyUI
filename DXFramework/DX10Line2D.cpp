#include "stdafx.h"
#include "DX10Line2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10Line2D, DX10Element2D);

	DX10Line2D::DX10Line2D()
	{
	}

	DX10Line2D::~DX10Line2D()
	{
	}

	BOOL DX10Line2D::Create(DX10& dx10)
	{
		return TRUE;
	}
	void DX10Line2D::Destory()
	{
		m_vertex.Release();
		m_index.Release();
		m_vertexTypes.Release();
	}
	DWORD DX10Line2D::GetIndexs() const
	{
		return static_cast<DWORD>(m_vertexTypes.GetSize());
	}
	BOOL DX10Line2D::DrawLine(DX10& dx10, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color)
	{
		if (m_vertexTypes.GetSize() != count)
		{
			Destory();
			m_vertexTypes.Reset(count);
			if (!m_vertexTypes)
				return FALSE;
			ZeroMemory(m_vertexTypes.Ptr(), (sizeof(VERTEXTYPE) * count));
			D3D10_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(VERTEXTYPE) * count;
			desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA data;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = m_vertexTypes.Ptr();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			HRESULT hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
			if (hRes != S_OK)
				return FALSE;
			TinyScopedArray<ULONG> indices(new ULONG[count]);
			if (!indices)
				return FALSE;
			ZeroMemory(indices.Ptr(), (sizeof(ULONG) * count));
			for (INT i = 0; i < count; i++)
			{
				indices[i] = i;
			}
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D10_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(ULONG) * count;
			desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = indices.Ptr();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_index);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx10.GetD3D()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < count; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		BYTE* bits = NULL;
		HRESULT hRes = m_vertex->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, (void*)m_vertexTypes.Ptr(), sizeof(VERTEXTYPE) * count);
		m_vertex->Unmap();
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_index, DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		return TRUE;
	}
}