#include "stdafx.h"
#include "D3D10VertexBuffer.h"

namespace D3D
{
	CD3D10VertexBuffer::CD3D10VertexBuffer()
	{
		
	}


	CD3D10VertexBuffer::~CD3D10VertexBuffer()
	{
	}
	BOOL CD3D10VertexBuffer::Create(BOOL bStatic)
	{
		D3D10_BUFFER_DESC bd;
		D3D10_SUBRESOURCE_DATA srd;
		::ZeroMemory(&bd, sizeof(bd));
		::ZeroMemory(&srd, sizeof(srd));
		bd.Usage = bStatic ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
		bd.CPUAccessFlags = bStatic ? 0 : D3D10_CPU_ACCESS_WRITE;
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		return TRUE;
		/*bd.ByteWidth = sizeof(CVector3D)*buf->numVerts;
		srd.pSysMem = vbData->VertList.Array();*/
	}
}
