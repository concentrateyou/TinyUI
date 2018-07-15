#include "stdafx.h"
#include "DX10Rectangle2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10Rectangle2D, DX10Element2D);

	DX10Rectangle2D::DX10Rectangle2D()
		:m_index(0)
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 4));
	}
	DX10Rectangle2D::~DX10Rectangle2D()
	{
	}
	BOOL DX10Rectangle2D::Create(DX10& dx10)
	{
		Destory();
		VERTEXTYPE vertexTypes[4];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 4));
		D3D10_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 4;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
		if (hRes != S_OK)
			return FALSE;
		ULONG indices1[5];
		ZeroMemory(indices1, (sizeof(ULONG) * 5));
		indices1[0] = 0;
		indices1[1] = 1;
		indices1[2] = 2;
		indices1[3] = 3;
		indices1[4] = 0;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 5;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices1;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		if (dx10.GetD3D()->CreateBuffer(&desc, &data, &m_indexs[0]) != S_OK)
			return FALSE;
		ULONG indices2[6];
		ZeroMemory(indices2, (sizeof(ULONG) * 6));
		indices2[0] = 0;
		indices2[1] = 1;
		indices2[2] = 2;
		indices2[3] = 2;
		indices2[4] = 3;
		indices2[5] = 0;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices2;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		if (dx10.GetD3D()->CreateBuffer(&desc, &data, &m_indexs[1]) != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX10Rectangle2D::Destory()
	{
		m_vertex.Release();
		m_indexs[0].Release();
		m_indexs[1].Release();
	}
	DWORD DX10Rectangle2D::GetIndexs() const
	{
		return m_index;
	}

	BOOL DX10Rectangle2D::DrawRectangle(DX10& dx10, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx10.GetD3D()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		BYTE* bits = NULL;
		HRESULT hRes = m_vertex->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 4);
		m_vertex->Unmap();
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_indexs[0], DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		m_index = 5;
		return TRUE;
	}
	BOOL DX10Rectangle2D::FillRectangle(DX10& dx10, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx10.GetD3D()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		BYTE* bits = NULL;
		HRESULT hRes = m_vertex->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 4);
		m_vertex->Unmap();
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_indexs[1], DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_index = 6;
		return TRUE;
	}
}