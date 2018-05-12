#include "stdafx.h"
#include "DX11Line2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Line2D, DX11Element2D);

	DX11Line2D::DX11Line2D()
	{
	}

	DX11Line2D::~DX11Line2D()
	{
	}

	BOOL DX11Line2D::Create(DX11& dx11, Line2D* lines, INT count)
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
		m_vertices.Reset(new VERTEXTYPE[count]);
		ASSERT(m_vertices);
		for (INT i = 0; i < count; i++)
		{
			m_vertices[i].position = XMFLOAT3(lines[i].pos.x, lines[i].pos.y, 0.0F);
			m_vertices[i].texture = XMFLOAT2(0.0F, 0.0F);
			m_vertices[i].color = lines[i].color;
		}
		return TRUE;
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