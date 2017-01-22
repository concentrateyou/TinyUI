#include "stdafx.h"
#include "DX11Rectangle.h"


namespace DXFramework
{
	DX11Rectangle::DX11Rectangle()
	{
	}

	DX11Rectangle::~DX11Rectangle()
	{
	}
	INT	DX11Rectangle::GetIndexCount() const
	{
		return 6;
	}
	BOOL DX11Rectangle::Create(DX11& dx11)
	{
		if (!Initialize(dx11))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Rectangle::Initialize(DX11& dx11)
	{
		D3D11_BUFFER_DESC	vertexBufferDesc = { 0 };
		D3D11_BUFFER_DESC	indexBufferDesc = { 0 };
		D3D11_SUBRESOURCE_DATA	vertexData = { 0 };
		D3D11_SUBRESOURCE_DATA	indexData = { 0 };
		INT indexCount = GetIndexCount();
		INT vertexCount = indexCount;
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[vertexCount]);
		TinyScopedArray<ULONG> indices(new ULONG[indexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
		for (INT i = 0; i < indexCount; i++)
		{
			indices[i] = i;
		}
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;
		vertexData.pSysMem = vertices.Ptr();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hRes))
			return FALSE;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = indices.Ptr();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hRes))
			return FALSE;
		m_vertices.Reset(new VERTEXTYPE[vertexCount]);
		return TRUE;
	}
	BOOL DX11Rectangle::Render(DX11& dx11)
	{
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
	BOOL DX11Rectangle::SetRectangle(DX11& dx11, const TinyRectangle& rectangle)
	{
		m_rectangle = rectangle;
		TinySize scale = m_rectangle.Size();
		TinyPoint pos = m_rectangle.Position();
		FLOAT left = 0.0F;
		FLOAT right = 0.0F;
		FLOAT top = 0.0F;
		FLOAT bottom = 0.0F;
		TinySize size = dx11.GetSize();
		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)pos.x;
		right = left + (FLOAT)scale.cx;
		top = (FLOAT)(size.cy / 2) - (FLOAT)pos.y;
		bottom = top - (FLOAT)scale.cy;
		INT vertexCount = GetIndexCount();
		m_vertices[0].position = D3DXVECTOR3(left, top, 0.0F);
		m_vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[1].position = D3DXVECTOR3(right, bottom, 0.0F);
		m_vertices[1].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[2].position = D3DXVECTOR3(left, bottom, 0.0F);
		m_vertices[2].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[3].position = D3DXVECTOR3(left, top, 0.0F);
		m_vertices[3].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[4].position = D3DXVECTOR3(right, top, 0.0F);
		m_vertices[4].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[5].position = D3DXVECTOR3(right, bottom, 0.0F);
		m_vertices[5].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hRes))
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertices.Ptr(), sizeof(VERTEXTYPE) * vertexCount);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		return TRUE;
	}
	void DX11Rectangle::Destory()
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
	}
}