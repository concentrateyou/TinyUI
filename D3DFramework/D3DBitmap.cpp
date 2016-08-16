#include "stdafx.h"
#include "D3DBitmap.h"

namespace DX
{
	D3DBitmap::D3DBitmap()
	{
	}


	D3DBitmap::~D3DBitmap()
	{
	}
	BOOL D3DBitmap::Initialize(ID3D10Device* device, INT cx, INT cy, HANDLE hResource)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
		m_texture.Initialize(device, hResource);
	}
	BOOL D3DBitmap::Initialize(ID3D10Device* device, INT cx, INT cy, CHAR* pzFile)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
	}
	BOOL D3DBitmap::Uninitialize()
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
	}
	BOOL D3DBitmap::Render(ID3D10Device*, INT x, INT y)
	{

	}
	BOOL D3DBitmap::InitializeBuffers(ID3D10Device* device)
	{
		D3D10_BUFFER_DESC vertexBufferDesc;
		D3D10_BUFFER_DESC indexBufferDesc;
		D3D10_SUBRESOURCE_DATA vertexData;
		D3D10_SUBRESOURCE_DATA indexData;
		m_vertexCount = 6;
		m_indexCount = m_vertexCount;
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[m_vertexCount]);
		TinyScopedArray<ULONG> indices(new ULONG[m_indexCount]);
		memset(&vertices, 0, (sizeof(VERTEXTYPE) * m_vertexCount));
		for (INT i = 0; i < m_indexCount; i++)
		{
			indices[i] = i;
		}
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexData.pSysMem = &vertices;
		HRESULT hRes = S_OK;
		hRes = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hRes))
			return FALSE;
		indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = &indices;
		hRes = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	ID3D10ShaderResourceView* D3DBitmap::GetTexture()
	{

	}
	BOOL D3DBitmap::LoadTexture(ID3D10Device*, CHAR* pzFile)
	{

	}
	BOOL D3DBitmap::LoadTexture(ID3D10Device* device, HANDLE hResource)
	{

	}
}