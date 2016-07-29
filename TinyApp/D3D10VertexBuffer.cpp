#include "stdafx.h"
#include "D3D10Device.h"
#include "D3D10VertexBuffer.h"

namespace D3D
{
	CD3D10VertexBuffer::CD3D10VertexBuffer()
	{

	}


	CD3D10VertexBuffer::~CD3D10VertexBuffer()
	{
	}
	BOOL CD3D10VertexBuffer::Create(CD3D10Device* device, BOOL bStatic)
	{
		ASSERT(device);
		D3D10_BUFFER_DESC dbd;
		D3D10_SUBRESOURCE_DATA dsrd;
		::ZeroMemory(&dbd, sizeof(dbd));
		::ZeroMemory(&dsrd, sizeof(dsrd));
		dbd.Usage = bStatic ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
		dbd.CPUAccessFlags = bStatic ? 0 : D3D10_CPU_ACCESS_WRITE;
		dbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		dbd.ByteWidth = sizeof(D3DXVECTOR4) * 4;
		dsrd.pSysMem = m_vertexs;
		if (FAILED(device->GetD3D()->CreateBuffer(&dbd, &dsrd, &m_vertexBuffer)))
		{
			return FALSE;
		}
		dbd.ByteWidth = sizeof(D3DXVECTOR2) * 4;
		dsrd.pSysMem = m_textureVertexs;
		if (FAILED(device->GetD3D()->CreateBuffer(&dbd, &dsrd, &m_textureVertexBuffer)))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL CD3D10VertexBuffer::Flush()
	{
		BYTE *ps = NULL;
		if (FAILED(m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&ps)))
		{
			return FALSE;
		}
		memcpy(ps, m_vertexs, sizeof(D3DXVECTOR4) * 4);
		m_vertexBuffer->Unmap();
		if (FAILED(m_textureVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&ps)))
		{
			return FALSE;
		}
		memcpy(ps, m_textureVertexs, sizeof(D3DXVECTOR2) * 4);
		m_textureVertexBuffer->Unmap();
		return TRUE;
	}
}
