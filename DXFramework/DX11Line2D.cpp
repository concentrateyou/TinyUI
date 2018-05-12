#include "stdafx.h"
#include "DX11Line2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Line2D, DX11Element2D);

	DX11Line2D::DX11Line2D()
		:m_count(0)
	{
	}

	DX11Line2D::~DX11Line2D()
	{
	}
	BOOL DX11Line2D::Create(DX11& dx11, XMFLOAT2* points, DWORD count, XMFLOAT4 color)
	{
		D3D11_BUFFER_DESC	vertexBufferDesc = { 0 };
		D3D11_BUFFER_DESC	indexBufferDesc = { 0 };
		D3D11_SUBRESOURCE_DATA	vertexData = { 0 };
		D3D11_SUBRESOURCE_DATA	indexData = { 0 };
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[count]);
		ASSERT(vertices);
		TinyScopedArray<ULONG> indices(new ULONG[count]);
		ASSERT(indices);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * count));
		for (INT i = 0; i < count; i++)
		{
			indices[i] = i;
		}
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;
		vertexData.pSysMem = vertices.Ptr();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexs);
		if (hRes != S_OK)
			return FALSE;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = indices.Ptr();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexs);
		if (hRes != S_OK)
			return FALSE;
		m_vertices.Reset(count);
		ASSERT(m_vertices);
		for (INT i = 0; i < count; i++)
		{
			m_vertices[i].position = XMFLOAT3(points[i].x, points[i].y, 0.0F);
			m_vertices[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hRes = dx11.GetImmediateContext()->Map(m_vertexs, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertices.Ptr(), sizeof(VERTEXTYPE) * count);
		dx11.GetImmediateContext()->Unmap(m_vertexs, 0);
		m_count = count;
		return TRUE;
	}
	DWORD DX11Line2D::GetCount() const
	{
		return m_count;
	}
	BOOL DX11Line2D::Process(DX11& dx11)
	{
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexs, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexs, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		return TRUE;
	}
}