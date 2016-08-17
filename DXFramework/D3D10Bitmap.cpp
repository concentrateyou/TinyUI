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
	BOOL D3D10Bitmap::Initialize(ID3D10Device* device, INT cx, INT cy, HANDLE hResource)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
		return m_texture.Initialize(device, hResource);
	}
	BOOL D3D10Bitmap::Initialize(ID3D10Device* device, INT cx, INT cy, CHAR* pzFile)
	{
		m_previousPosX = -1;
		m_previousPosY = -1;
		m_cx = cx;
		m_cy = cy;
		return m_texture.Initialize(device, pzFile);
	}
	BOOL D3D10Bitmap::Render(ID3D10Device*, INT x, INT y)
	{
		return TRUE;
	}
	BOOL D3D10Bitmap::InitializeBuffers(ID3D10Device* device)
	{
		D3D10_BUFFER_DESC vertexBufferDesc;
		D3D10_BUFFER_DESC indexBufferDesc;
		D3D10_SUBRESOURCE_DATA vertexData;
		D3D10_SUBRESOURCE_DATA indexData;
		//两个三角形组成矩形所以6个点
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[6]);
		TinyScopedArray<ULONG> indexs(new ULONG[6]);
		memset(&vertices, 0, (sizeof(VERTEXTYPE) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indexs[i] = i;
		}
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexData.pSysMem = &vertices;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
			return FALSE;
		indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * 6;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = &indexs;
		if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
			return FALSE;
		return TRUE;
	}
	ID3D10ShaderResourceView* D3D10Bitmap::GetTexture()
	{
		return m_texture.GetTexture();
	}
	BOOL D3D10Bitmap::LoadTexture(ID3D10Device* device, CHAR* pzFile)
	{
		return m_texture.Initialize(device, pzFile);
	}
	BOOL D3D10Bitmap::LoadTexture(ID3D10Device* device, HANDLE hResource)
	{
		return m_texture.Initialize(device, hResource);
	}
}