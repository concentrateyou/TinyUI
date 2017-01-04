#include "stdafx.h"
#include "DX11Image.h"

namespace DXFramework
{
	DX11Image::DX11Image()
		:m_scaleX(0),
		m_scaleY(0),
		m_positionX(-1),
		m_positionY(-1),
		m_vertexCount(0),
		m_indexCount(0)
	{
	}
	DX11Image::~DX11Image()
	{
	}
	void DX11Image::SetPosition(INT x, INT y)
	{
		m_positionX = x;
		m_positionY = y;
	}
	TinySize DX11Image::GetScale() const
	{
		return TinySize(m_scaleX, m_scaleY);
	}
	void DX11Image::GetScale(INT scaleX, INT scaleY)
	{
		m_scaleX = scaleX;
		m_scaleY = scaleY;
	}
	BOOL DX11Image::Create(const DX11& dx11, INT cx, INT cy, INT scaleX, INT scaleY, BYTE* pData)
	{
		if (!Initialize(dx11))
			return FALSE;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
		return m_texture.CreateTexture(dx11, cx, cy, pData);
	}
	BOOL DX11Image::BitBlt(const DX11& dx11, const BYTE* pData)
	{
		ASSERT(m_texture.IsValid());
		HDC hDC = NULL;
		BOOL bRes = m_texture.GetDC(hDC);
		if (!bRes)
			return FALSE;
		TinySize size = m_texture.GetSize();
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = size.cx;
		bmi.bmiHeader.biHeight = size.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = size.cx * size.cy * 4;
		BYTE* pvBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
		memcpy(pvBits, pData, bmi.bmiHeader.biSizeImage);
		TinyUI::TinyMemDC mdc(hDC, hBitmap);
		::BitBlt(hDC, 0, 0, size.cx, size.cy, mdc, 0, 0, SRCCOPY);
		SAFE_DELETE_OBJECT(hBitmap);
		bRes = m_texture.ReleaseDC();
		if (!bRes)
			return FALSE;
		return TRUE;
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
		m_positionX = -1;
		m_positionY = -1;
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
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
		FLOAT left = 0.0F;
		FLOAT right = 0.0F;
		FLOAT top = 0.0F;
		FLOAT bottom = 0.0F;
		if ((positionX == m_positionX) && (positionY == m_positionY))
			return TRUE;
		m_positionX = positionX;
		m_positionY = positionY;
		TinySize size = dx11.GetSize();

		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)positionX;
		right = left + (FLOAT)m_scaleX;
		top = (FLOAT)(size.cy / 2) - (FLOAT)positionY;
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
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hRes))
			return FALSE;
		memcpy(mappedResource.pData, (void*)vertices.Ptr(), sizeof(VERTEXTYPE) * m_vertexCount);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
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
	BOOL DX11Image::IsValid() const
	{
		return m_texture.IsValid();
	}
	BOOL DX11Image::Render(const DX11& dx11, INT positionX, INT positionY)
	{
		if (!this->Update(dx11, positionX, positionY))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	SharedTexture::SharedTexture()
		:m_hWND(NULL)
	{

	}
	SharedTexture::~SharedTexture()
	{

	}
	BOOL SharedTexture::Initialize(const DX11& dx11, INT scaleX, INT scaleY)
	{
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return FALSE;
			if (!m_textureMemery.Map(0, sizeof(SharedTextureDATA)))
				return FALSE;
		}
		SharedTextureDATA* pTexture = reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
		if (!pTexture)
			return FALSE;
		if (!m_image.Load(dx11, pTexture->TextureHandle, scaleX, scaleY))
			return FALSE;
		return TRUE;
	}
	DX11Image& SharedTexture::GetTexture()
	{
		return m_image;
	}
}