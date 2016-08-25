#include "stdafx.h"
#include "DX10Image.h"

namespace DXFramework
{
	DX10Image::DX10Image()
	{
	}

	DX10Image::~DX10Image()
	{
	}
	BOOL DX10Image::Load(const DX10& dx10, HANDLE hResource)
	{

	}
	BOOL DX10Image::Initialize(const DX10& dx10)
	{
		D3D10_BUFFER_DESC vertexBufferDesc;
		D3D10_BUFFER_DESC indexBufferDesc;
		D3D10_SUBRESOURCE_DATA vertexData;
		D3D10_SUBRESOURCE_DATA indexData;
		m_vertexCount = 6;
		m_indexCount = m_vertexCount;
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[m_vertexCount]);
		TinyScopedArray<ULONG> indices(new ULONG[m_indexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * m_vertexCount));
		for (INT i = 0; i < m_indexCount; i++)
			indices[i] = i;
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexData.pSysMem = vertices.Ptr();
		HRESULT hRes = dx10.GetD3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hRes))
			return FALSE;
		indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = indices.Ptr();
		hRes = dx10.GetD3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}

	BOOL DX10Image::Update(const DX10& dx10, INT positionX, int positionY)
	{
		FLOAT left;
		FLOAT right;
		FLOAT top;
		FLOAT bottom;
		VERTEXTYPE* vertices;
		if ((positionX == m_previousPosX) &&
			(positionY == m_previousPosY))
			return TRUE;
		m_previousPosX = positionX;
		m_previousPosY = positionY;
		SIZE size = dx10.GetSize();
		SIZE imageSize = m_dx10Texture.GetSize();
		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)positionX;
		right = left + (FLOAT)imageSize.cx;
		top = (FLOAT)(size.cx / 2) - (FLOAT)positionY;
		bottom = top - (float)imageSize.cy;
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[m_vertexCount]);
		vertices[0].position = D3DXVECTOR3(left, top, 0.0F);
		vertices[0].texture = D3DXVECTOR2(0.0F, 0.0F);
		vertices[1].position = D3DXVECTOR3(right, bottom, 0.0F);
		vertices[1].texture = D3DXVECTOR2(1.0F, 1.0f);
		vertices[2].position = D3DXVECTOR3(left, bottom, 0.0F);
		vertices[2].texture = D3DXVECTOR2(0.0F, 1.0f);
		vertices[3].position = D3DXVECTOR3(left, top, 0.0F);
		vertices[3].texture = D3DXVECTOR2(0.0F, 0.0F);
		vertices[4].position = D3DXVECTOR3(right, top, 0.0F);
		vertices[4].texture = D3DXVECTOR2(1.0F, 0.0F);
		vertices[5].position = D3DXVECTOR3(right, bottom, 0.0F);
		vertices[5].texture = D3DXVECTOR2(1.0F, 1.0F);
		void* pData = 0;
		HRESULT hRes = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pData);
		if (FAILED(hRes))
			return FALSE;
		memcpy(pData, (void*)vertices, (sizeof(VERTEXTYPE) * m_vertexCount));
		m_vertexBuffer->Unmap();
		return TRUE;
	}
}