#include "stdafx.h"
#include "DX11Line2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Line2D, DX11Element2D);

	DX11Line2D::DX11Line2D()
		:m_indexs(0)
	{
	}

	DX11Line2D::~DX11Line2D()
	{
	}

	BOOL DX11Line2D::Create(DX11& dx11, XMFLOAT2* points, DWORD count, XMFLOAT4 color)
	{
		Destory();
		TinyScopedArray<VERTEXTYPE> vertexTypes(new VERTEXTYPE[count]);
		if (!vertexTypes)
			return FALSE;
		ZeroMemory(vertexTypes.Ptr(), (sizeof(VERTEXTYPE) * count));
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * count;
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
		m_vertexes.Reset(count);
		if (!m_vertexes)
			return FALSE;
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT views = 1;
		dx11.GetImmediateContext()->RSGetViewports(&views, &vp);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		for (INT i = 0; i < count; i++)
		{
			m_vertexes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexes.Ptr(), sizeof(VERTEXTYPE) * count);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		//////////////////////////////////////////////////////////////////////////
		TinyScopedArray<ULONG> indices(new ULONG[count]);
		ASSERT(indices);
		ZeroMemory(indices.Ptr(), (sizeof(ULONG) * count));
		for (INT i = 0; i < count; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices.Ptr();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_indexBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_indexs = count;
		return TRUE;
	}
	void DX11Line2D::Destory()
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
		m_vertexes.Release();
	}
	DWORD DX11Line2D::GetIndexs() const
	{
		return m_indexs;
	}
	BOOL DX11Line2D::SetColor(DX11& dx11, XMFLOAT4 color)
	{
		UINT count = m_vertexes.GetSize();
		for (UINT i = 0; i < count; i++)
		{
			m_vertexes[i].color = color;
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexes.Ptr(), sizeof(VERTEXTYPE) * count);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		return TRUE;
	}
	BOOL DX11Line2D::Process(DX11& dx11)
	{
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		return TRUE;
	}
}