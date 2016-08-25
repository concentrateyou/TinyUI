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
		if (!Initialize(dx10))
			return FALSE;
		return m_texture.LoadTexture(dx10, hResource);
	}
	BOOL DX10Image::Load(const DX10& dx10, const CHAR* pzFile)
	{
		if (!Initialize(dx10))
			return FALSE;
		return m_texture.LoadTexture(dx10, pzFile);
	}
	BOOL DX10Image::Load(const DX10& dx10, const BYTE* pData, INT size)
	{
		if (!Initialize(dx10))
			return FALSE;
		return m_texture.LoadTexture(dx10, pData, size);
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
	BOOL DX10Image::Update(const DX10& dx10, INT positionX, INT positionY)
	{
		FLOAT left;
		FLOAT right;
		FLOAT top;
		FLOAT bottom;
		if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
			return TRUE;
		m_previousPosX = positionX;
		m_previousPosY = positionY;
		SIZE size = dx10.GetSize();
		SIZE imageSize = m_texture.GetSize();
		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)positionX;
		right = left + (FLOAT)imageSize.cx;
		top = (FLOAT)(size.cx / 2) - (FLOAT)positionY;
		bottom = top - (FLOAT)imageSize.cy;
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
		void* pData = NULL;
		HRESULT hRes = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pData);
		if (FAILED(hRes))
			return FALSE;
		memcpy(pData, (void*)vertices.Ptr(), (sizeof(VERTEXTYPE)*m_vertexCount));
		m_vertexBuffer->Unmap();
		return TRUE;
	}
	INT	 DX10Image::GetIndexCount() const
	{
		return m_indexCount;
	}
	ID3D10Texture2D* DX10Image::GetTexture2D() const
	{
		return m_texture.GetTexture2D();
	}
	BOOL DX10Image::Render(const DX10& dx10, int positionX, int positionY)
	{
		if (!this->Update(dx10, positionX, positionY))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
}