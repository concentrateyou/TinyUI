#include "stdafx.h"
#include "D3D10Bitmap.h"

namespace DXFramework
{
	D3D10Bitmap::D3D10Bitmap()
	{
	}


	D3D10Bitmap::~D3D10Bitmap()
	{
	}
	BOOL D3D10Bitmap::Initialize(ID3D10Device* pDevice, INT cx, INT cy, HANDLE hResource)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
		if (!InitializeBuffers(pDevice))
			return FALSE;
		return m_texture.Initialize(pDevice, hResource);
	}
	BOOL D3D10Bitmap::Initialize(ID3D10Device* pDevice, INT cx, INT cy, CHAR* pzFile)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
		if (!InitializeBuffers(pDevice))
			return FALSE;
		return m_texture.Initialize(pDevice, pzFile);
	}
	BOOL D3D10Bitmap::Render(ID3D10Device* pDevice, INT x, INT y)
	{
		if (!UpdateBuffers(x, y))
			return FALSE;
		RenderBuffers(pDevice);
		return TRUE;
	}
	BOOL D3D10Bitmap::InitializeBuffers(ID3D10Device* pDevice)
	{
		//两个三角形组成矩形所以6个点
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[6]);
		TinyScopedArray<ULONG> indexs(new ULONG[6]);
		memset(vertices.Ptr(), 0, (sizeof(VERTEXTYPE) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indexs[i] = i;
		}
		D3D10_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = &vertices;
		if (FAILED(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
			return FALSE;
		D3D10_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * 6;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = &indexs;
		if (FAILED(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
			return FALSE;
		return TRUE;
	}
	BOOL D3D10Bitmap::UpdateBuffers(INT positionX, INT positionY)
	{
		FLOAT left, right, top, bottom;
		HRESULT hRes;
		if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
			return TRUE;
		m_previousPosX = positionX;
		m_previousPosY = positionY;
		left = (FLOAT)((m_cx / 2) * -1) + (FLOAT)positionX;
		right = left + (FLOAT)m_texture.GetSize().cx;
		top = (FLOAT)(m_cy / 2) - (FLOAT)positionY;
		bottom = top - (FLOAT)m_texture.GetSize().cy;
		TinyUI::TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[6]);
		if (!vertices.IsEmpty())
		{
			vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
			vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
			vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
			vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
			vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
			vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
			vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
			vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
			vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
			vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);
			vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
			vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);
			void* verticesPtr = 0;
			hRes = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
			if (FAILED(hRes))
				return FALSE;
			memcpy(verticesPtr, (void*)vertices.Ptr(), (sizeof(VERTEXTYPE) * 6));
			m_vertexBuffer->Unmap();
			return TRUE;
		}
		return FALSE;
	}
	void D3D10Bitmap::RenderBuffers(ID3D10Device* pDevice)
	{
		ASSERT(pDevice);
		UINT stride;
		UINT offset;
		stride = sizeof(VERTEXTYPE);
		offset = 0;
		pDevice->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		pDevice->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	ID3D10ShaderResourceView* D3D10Bitmap::GetTexture()
	{
		return m_texture.GetTexture();
	}
	BOOL D3D10Bitmap::LoadTexture(ID3D10Device* pDevice, CHAR* pzFile)
	{
		return m_texture.Initialize(pDevice, pzFile);
	}
	BOOL D3D10Bitmap::LoadTexture(ID3D10Device* pDevice, HANDLE hResource)
	{
		return m_texture.Initialize(pDevice, hResource);
	}
}