#include "stdafx.h"
#include "DX11Image.h"

namespace DXFramework
{
	DX11Image::DX11Image()
	{
	}
	DX11Image::~DX11Image()
	{
	}
	BOOL DX11Image::Create(const DX11& dx11, const TinySize& size, BYTE* bits)
	{

		if (!Initialize(dx11))
			return FALSE;
		if (m_texture.CreateCompatible(dx11, m_size.cx, m_size.cy, bits))
		{
			m_size = m_texture.GetSize();
			m_scale = m_size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image::BitBlt(const DX11& dx11, const BYTE* bits)
	{
		ASSERT(m_texture.IsValid());
		HDC hDC = NULL;
		BOOL bRes = m_texture.GetDC(hDC);
		if (!bRes)
			return FALSE;
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_size.cx;
		bmi.bmiHeader.biHeight = m_size.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 4;
		BYTE* pvBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
		memcpy(pvBits, bits, bmi.bmiHeader.biSizeImage);
		TinyUI::TinyMemDC mdc(hDC, hBitmap);
		::BitBlt(hDC, 0, 0, m_size.cx, m_size.cy, mdc, 0, 0, SRCCOPY);
		SAFE_DELETE_OBJECT(hBitmap);
		bRes = m_texture.ReleaseDC();
		if (!bRes)
			return FALSE;
		return TRUE;
	}
	BOOL DX11Image::Load(const DX11& dx11, HANDLE hResource)
	{
		if (!Initialize(dx11))
			return FALSE;
		if (m_texture.Load(dx11, hResource))
		{
			m_size = m_texture.GetSize();
			RECT s = { 0 };
			GetClientRect(dx11.GetHWND(), &s);
			m_scale.cx = TO_CX(s);
			m_scale.cy = TO_CY(s);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image::Load(const DX11& dx11, const CHAR* pzFile)
	{
		if (!Initialize(dx11))
			return FALSE;
		if (m_texture.Load(dx11, pzFile))
		{
			m_size = m_texture.GetSize();
			m_scale = m_size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image::Load(const DX11& dx11, const BYTE* bits, DWORD dwSize)
	{
		if (!Initialize(dx11))
			return FALSE;
		if (m_texture.Load(dx11, bits, dwSize))
		{
			m_size = m_texture.GetSize();
			m_scale = m_size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image::Initialize(const DX11& dx11)
	{
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
		D3D11_BUFFER_DESC		vertexBufferDesc;
		D3D11_BUFFER_DESC		indexBufferDesc;
		D3D11_SUBRESOURCE_DATA	vertexData;
		D3D11_SUBRESOURCE_DATA	indexData;
		INT vertexCount = GetIndexCount();
		INT indexCount = GetIndexCount();
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[vertexCount]);
		TinyScopedArray<ULONG> indices(new ULONG[indexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
		for (INT i = 0; i < indexCount; i++)
		{
			indices[i] = i;
		}
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VERTEXTYPE) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexData.pSysMem = vertices.Ptr();
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hRes))
			return FALSE;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(ULONG) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexData.pSysMem = indices.Ptr();
		hRes = dx11.GetD3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Image::Update(const DX11& dx11)
	{
		FLOAT left = 0.0F;
		FLOAT right = 0.0F;
		FLOAT top = 0.0F;
		FLOAT bottom = 0.0F;
		TinySize size = dx11.GetSize();
		left = (FLOAT)((size.cx / 2) * -1) + (FLOAT)m_position.x;
		right = left + (FLOAT)m_scale.cx;
		top = (FLOAT)(size.cy / 2) - (FLOAT)m_position.y;
		bottom = top - (FLOAT)m_scale.cy;
		INT vertexCount = GetIndexCount();
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[vertexCount]);
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
		memcpy(mappedResource.pData, (void*)vertices.Ptr(), sizeof(VERTEXTYPE) * vertexCount);
		dx11.GetImmediateContext()->Unmap(m_vertexBuffer, 0);
		return TRUE;
	}

	BOOL DX11Image::BeginScene()
	{
		return TRUE;
	}

	void DX11Image::EndScene()
	{

	}

	DX11Texture* DX11Image::GetTexture()
	{
		return &m_texture;
	}

	LPCSTR DX11Image::GetClassName()
	{
		return TEXT("DX11Image");
	}
	ElementType	 DX11Image::GetElementType() const
	{
		return IMAGE;
	}
	BOOL DX11Image::IsValid() const
	{
		return m_texture.IsValid();
	}
	BOOL DX11Image::Render(const DX11& dx11)
	{
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}

	INT DX11Image::GetIndexCount() const
	{
		return 6;
	}
}