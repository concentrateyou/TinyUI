#include "stdafx.h"
#include "DX11Image.h"

namespace DXFramework
{
	DX11Image::DX11Image()
		:m_scaleX(0),
		m_scaleY(0),
		m_previousPosX(-1),
		m_previousPosY(-1),
		m_vertexCount(0),
		m_indexCount(0)
	{
	}
	DX11Image::~DX11Image()
	{
	}
	BOOL DX11Image::Create(const DX11& dx11, INT cx, INT cy, INT scaleX, INT scaleY)
	{
		if (!Initialize(dx11))
			return FALSE;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
		return m_texture.CreateTexture(dx11, cx, cy, DXGI_FORMAT_B8G8R8X8_UNORM, NULL);
	}
	BOOL DX11Image::FillImage(const DX11& dx11, const BYTE* pBits, INT cx, INT cy)
	{
		ASSERT(m_texture.IsValid());
		return m_texture.FillTexture(dx11, pBits, cx, cy);
	}
	BOOL DX11Image::Load(const DX11& dx11, HANDLE hResource, INT scaleX, INT scaleY)
	{
		if (!Initialize(dx11))
			return FALSE;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
		return m_texture.LoadTexture(dx11, hResource);
	}
	BOOL DX11Image::Load(const DX11& dx11, const CHAR* pzFile, INT scaleX, INT scaleY)
	{
		if (!Initialize(dx11))
			return FALSE;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
		return m_texture.LoadTexture(dx11, pzFile);
	}
	BOOL DX11Image::Load(const DX11& dx11, const BYTE* pData, DWORD dwSize, INT scaleX, INT scaleY)
	{
		if (!Initialize(dx11))
			return FALSE;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
		return m_texture.LoadTexture(dx11, pData, dwSize);
	}
	BOOL DX11Image::Initialize(const DX11& dx11)
	{
		D3D11_BUFFER_DESC		vertexBufferDesc;
		D3D11_BUFFER_DESC		indexBufferDesc;
		D3D11_SUBRESOURCE_DATA	vertexData;
		D3D11_SUBRESOURCE_DATA	indexData;
		m_vertexCount = 6;
		m_indexCount = m_vertexCount;
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[m_vertexCount]);
		TinyScopedArray<ULONG> indices(new ULONG[m_indexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * m_vertexCount));
		for (INT i = 0; i < m_indexCount; i++)
			indices[i] = i;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexData.pSysMem = vertices.Ptr();
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hRes))
			return FALSE;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = indices.Ptr();
		hRes = dx11.GetD3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Image::Update(const DX11& dx11, INT positionX, INT positionY)
	{
		FLOAT left;
		FLOAT right;
		FLOAT top;
		FLOAT bottom;
		if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
			return TRUE;
		m_previousPosX = positionX;
		m_previousPosY = positionY;
		TinySize size = dx11.GetSize();
		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)positionX;
		right = left + (FLOAT)m_scaleX;
		top = (FLOAT)(size.cx / 2) - (FLOAT)positionY;
		bottom = top - (FLOAT)m_scaleY;
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
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hRes))
			return FALSE;
		memcpy(mappedResource.pData, (void*)vertices.Ptr(), sizeof(VERTEXTYPE) * m_vertexCount);
		dx11.GetContext()->Unmap(m_vertexBuffer, 0);
		return TRUE;
	}
	INT	 DX11Image::GetIndexCount() const
	{
		return m_indexCount;
	}
	DX11Texture* DX11Image::GetTexture()
	{
		return &m_texture;
	}
	BOOL DX11Image::Render(const DX11& dx11, INT positionX, INT positionY)
	{
		if (!this->Update(dx11, positionX, positionY))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
}