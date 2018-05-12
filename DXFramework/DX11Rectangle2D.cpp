#include "stdafx.h"
#include "DX11Rectangle2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Rectangle2D, DX11Element2D);

	DX11Rectangle2D::DX11Rectangle2D()
		:m_indexs(0),
		m_topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
	{
	}
	DX11Rectangle2D::~DX11Rectangle2D()
	{
	}
	BOOL DX11Rectangle2D::Create(DX11& dx11, XMFLOAT2 points[4], XMFLOAT4 color)
	{
		Destory();
		TinyScopedArray<VERTEXTYPE> vertexTypes(new VERTEXTYPE[4]);
		if (!vertexTypes)
			return FALSE;
		ZeroMemory(vertexTypes.Ptr(), (sizeof(VERTEXTYPE) * 4));
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
		data.pSysMem = vertexTypes.Ptr();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_vertexBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_vertexes.Reset(new VERTEXTYPE[4]);
		if (!m_vertexes)
			return FALSE;
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx11.GetImmediateContext()->RSGetViewports(&count, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexes.Ptr(), sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		SetPrimitiveTopology(dx11, m_topology);
		memcpy(m_points, points, sizeof(XMFLOAT2) * 4);
		m_color = color;
		return TRUE;
	}
	void DX11Rectangle2D::Destory()
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
		m_vertexes.Reset(NULL);
	}
	BOOL DX11Rectangle2D::SetColor(DX11& dx11, XMFLOAT4 color)
	{
		if (!m_vertexes)
			return FALSE;
		for (INT i = 0; i < 4; i++)
		{
			m_vertexes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexes.Ptr(), sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		m_color = color;
		return TRUE;
	}
	BOOL DX11Rectangle2D::SetPoints(DX11& dx11, XMFLOAT2 points[4])
	{
		if (!m_vertexes)
			return FALSE;
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx11.GetImmediateContext()->RSGetViewports(&count, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < 4; i++)
		{
			m_vertexes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexes.Ptr(), sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		memcpy(m_points, points, sizeof(XMFLOAT2) * 4);
		return TRUE;
	}
	DWORD DX11Rectangle2D::GetIndexs() const
	{
		return m_indexs;
	}
	BOOL DX11Rectangle2D::SetPrimitiveTopology(DX11& dx11, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (m_topology != topology)
		{
			m_topology = topology;
			if (m_topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			{
				m_indexBuffer.Release();
				TinyScopedArray<ULONG> indices(new ULONG[5]);
				if (!indices)
					return FALSE;
				ZeroMemory(indices.Ptr(), (sizeof(ULONG) * 5));
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 2;
				indices[3] = 3;
				indices[4] = 0;
				D3D11_BUFFER_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = sizeof(ULONG) * 5;
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				D3D11_SUBRESOURCE_DATA data;
				ZeroMemory(&data, sizeof(data));
				data.pSysMem = indices.Ptr();
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;
				if (dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexBuffer) != S_OK)
					return FALSE;
				m_indexs = 5;
			}
			if (m_topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			{
				m_indexBuffer.Release();
				TinyScopedArray<ULONG> indices(new ULONG[6]);
				if (!indices)
					return FALSE;
				ZeroMemory(indices.Ptr(), (sizeof(ULONG) * 6));
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 2;
				indices[3] = 2;
				indices[4] = 3;
				indices[5] = 0;
				D3D11_BUFFER_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = sizeof(ULONG) * 6;
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				D3D11_SUBRESOURCE_DATA data;
				ZeroMemory(&data, sizeof(data));
				data.pSysMem = indices.Ptr();
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;
				if (dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexBuffer) != S_OK)
					return FALSE;
				m_indexs = 6;
			}
		}
		return TRUE;
	}
	BOOL DX11Rectangle2D::Process(DX11& dx11)
	{
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(m_topology);
		return TRUE;
	}
}