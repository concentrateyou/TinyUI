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
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[4]);
		ASSERT(vertices);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * 4));
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
		data.pSysMem = vertices.Ptr();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_vertexBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_vertices.Reset(new VERTEXTYPE[4]);
		ASSERT(m_vertices);
		for (INT i = 0; i < 4; i++)
		{
			m_vertices[i].position = XMFLOAT3(points[i].x - 0.5, points[i].y - 0.5, 0.0F);
			m_vertices[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertices.Ptr(), sizeof(VERTEXTYPE) * 4);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		SetPrimitiveTopology(dx11, m_topology);
		return TRUE;
	}
	void DX11Rectangle2D::Destory()
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
		m_vertices.Reset(NULL);
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
			HRESULT hRes = S_OK;
			//¾ØÐÎ¿ò
			if (m_topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			{
				m_indexBuffer.Release();
				TinyScopedArray<ULONG> indices(new ULONG[5]);
				ASSERT(indices);
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
				hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexBuffer);
				if (hRes != S_OK)
					return FALSE;
				m_indexs = 5;
			}
			//ÊµÐÄ¿ò
			if (m_topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			{
				m_indexBuffer.Release();
				TinyScopedArray<ULONG> indices(new ULONG[6]);
				ASSERT(indices);
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
				hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexBuffer);
				if (hRes != S_OK)
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