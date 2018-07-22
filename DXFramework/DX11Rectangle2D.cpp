#include "stdafx.h"
#include "DX11Rectangle2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Rectangle2D, DX11Element);

	DX11Rectangle2D::DX11Rectangle2D()
		:m_index(0)
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 4));
	}
	DX11Rectangle2D::~DX11Rectangle2D()
	{
	}
	BOOL DX11Rectangle2D::Create(DX11& dx11)
	{
		Destory();
		VERTEXTYPE vertexTypes[4];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 4));
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
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
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 5;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices1;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		if (dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexs[0]) != S_OK)
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
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices2;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		if (dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexs[1]) != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX11Rectangle2D::Destory()
	{
		m_vertex.Release();
		m_indexs[0].Release();
		m_indexs[1].Release();
	}
	DWORD DX11Rectangle2D::GetIndexs() const
	{
		return m_index;
	}

	BOOL DX11Rectangle2D::DrawRectangle(DX11& dx11, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx11.GetImmediateContext()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertex, 0);
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexs[0], DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		m_index = 5;
		return TRUE;
	}
	BOOL DX11Rectangle2D::FillRectangle(DX11& dx11, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx11.GetImmediateContext()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertex, 0);
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexs[1], DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_index = 6;
		return TRUE;
	}
}