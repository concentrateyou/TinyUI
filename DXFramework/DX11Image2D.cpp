#include "stdafx.h"
#include "DX11Image2D.h"
#include <algorithm>
#include <limits>
#pragma comment(lib,"gdiplus.lib")

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Image2D, DX11ImageElement2D);

	DX11Image2D::DX11Image2D()
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 6));
	}
	DX11Image2D::~DX11Image2D()
	{

	}
	BOOL DX11Image2D::Initialize(DX11& dx11)
	{
		VERTEXTYPE vertexTypes[6];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 6));
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA	data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
		if (hRes != S_OK)
			return FALSE;
		ULONG indices[6];
		ZeroMemory(indices, (sizeof(ULONG) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&desc, &data, &m_index);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11Image2D::Calculate(DX11& dx11)
	{
		if (!m_vertex)
			return FALSE;
		m_vertexTypes[0].position = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[0].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[0].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[1].position = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[1].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[1].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[2].position = XMFLOAT3(-m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[2].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[2].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[3].position = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[3].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[3].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[4].position = XMFLOAT3(m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[4].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[4].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[5].position = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[5].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[5].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx11.GetImmediateContext()->RSGetViewports(&count, &vp);
		XMFLOAT2 size(m_size.x * m_scale.x, m_size.y * m_scale.y);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, 0.0F);
		XMMATRIX rotation = XMMatrixRotationZ(((m_angle) * (D3DX_PI / 180.0)));
		XMMATRIX translation = XMMatrixTranslation(-center.x + size.x / 2 + m_translate.x, center.y - size.y / 2 - m_translate.y, 0.0F);
		XMMATRIX val = scaling * rotation * translation;
		XMMATRIX* ms = dx11.GetMatrixs();
		ms[1] *= val;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hRes = dx11.GetImmediateContext()->Map(m_vertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hRes != S_OK)
			return FALSE;
		memcpy(mappedResource.pData, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 6);
		dx11.GetImmediateContext()->Unmap(m_vertex, 0);
		return TRUE;
	}
	BOOL DX11Image2D::Create(DX11& dx11, D3D11_TEXTURE2D_DESC& desc)
	{
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Create(dx11, desc))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Create(DX11& dx11, ID3D11Texture2D* texture2D)
	{
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Create(dx11, texture2D))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Create(DX11& dx11, INT cx, INT cy, const BYTE* bits, BOOL bReadoly)
	{
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Create(dx11, cx, cy, bits, bReadoly))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Create(DX11& dx11, INT cx, INT cy)
	{
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Create(dx11, cx, cy))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Load(DX11& dx11, const BYTE* bits, LONG size)
	{
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Load(dx11, bits, size))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Load(DX11& dx11, HANDLE hResource)
	{
		if (!hResource)
			return FALSE;
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Load(dx11, hResource))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::Load(DX11& dx11, const CHAR* pzFile)
	{
		if (!PathFileExists(pzFile))
			return FALSE;
		Destory();
		if (!Initialize(dx11))
			return FALSE;
		if (DX11Texture2D::Load(dx11, pzFile))
		{
			D3D11_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	void DX11Image2D::Destory()
	{
		m_vertex.Release();
		m_index.Release();
		DX11Texture2D::Destory();
	}
	BOOL DX11Image2D::BitBlt(DX11& dx11, const BYTE* bits, LONG size, LONG linesize)
	{
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		if (!bits || DX11Texture2D::IsEmpty() || size != (sizeT.cx * sizeT.cy * 4))
			return FALSE;
		HDC hDC = NULL;
		if (!DX11Texture2D::GetDC(FALSE, hDC))
			return FALSE;
		LONG _linesize = sizeT.cx * 4;
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = sizeT.cx;
		bmi.bmiHeader.biHeight = sizeT.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = _linesize * sizeT.cy;
		BYTE* pBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pBits), NULL, 0);
		INT rowcopy = (_linesize < linesize) ? _linesize : linesize;
		if (linesize == rowcopy)
		{
			memcpy(pBits, bits, rowcopy * sizeT.cy);
		}
		else
		{
			for (INT y = 0; y < sizeT.cy; y++)
			{
				memcpy(pBits + (UINT)y * _linesize, bits + (UINT)y * linesize, rowcopy);
			}
		}
		TinyFramework::TinyMemDC mdc(hDC, hBitmap);
		::BitBlt(hDC, 0, 0, sizeT.cx, sizeT.cy, mdc, 0, 0, SRCCOPY);
		SAFE_DELETE_OBJECT(hBitmap);
		return DX11Texture2D::ReleaseDC();
	}
	BOOL DX11Image2D::BitBlt(DX11& dx11, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src)
	{
		if (DX11Texture2D::IsEmpty())
			return FALSE;
		HDC hDC = NULL;
		if (!DX11Texture2D::GetDC(TRUE, hDC))
			return FALSE;
		TinyFramework::TinyMemDC mdc(hDC, hBitmapSrc);
		::BitBlt(hDC, dst.left, dst.top, dst.Width(), dst.Height(), mdc, src.x, src.y, SRCCOPY | CAPTUREBLT);
		return DX11Texture2D::ReleaseDC();
	}
	BOOL DX11Image2D::BitBlt(DX11& dx11, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src)
	{
		if (DX11Texture2D::IsEmpty())
			return FALSE;
		HDC hDC = NULL;
		if (!DX11Texture2D::GetDC(TRUE, hDC))
			return FALSE;
		::BitBlt(hDC, dst.left, dst.top, dst.Width(), dst.Height(), hDCSrc, src.x, src.y, SRCCOPY | CAPTUREBLT);
		return DX11Texture2D::ReleaseDC();
	}
	BOOL DX11Image2D::Copy(DX11& dx11, D3D11_BOX* ps, const BYTE* bits, LONG size)
	{
		if (DX11Texture2D::IsEmpty() || !bits || size <= 0)
			return FALSE;
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		return DX11Texture2D::Copy(dx11, ps, bits, size, sizeT.cx * 4, sizeT.cx * sizeT.cy * 4);
	}
	BOOL DX11Image2D::Copy(DX11& dx11, const BYTE* bits, LONG size, UINT stride)
	{
		if (DX11Texture2D::IsEmpty() || !bits || size <= 0)
			return FALSE;
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		BYTE* bitso = NULL;
		UINT linesize = 0;
		UINT rowcopy = (stride < linesize) ? stride : linesize;
		D3D11_MAPPED_SUBRESOURCE ms;
		if (DX11Texture2D::Map(dx11, ms))
		{
			bitso = static_cast<BYTE*>(ms.pData);
			linesize = ms.RowPitch;
			UINT rowcopy = (stride < linesize) ? stride : linesize;
			if (linesize == rowcopy)
			{
				memcpy(bitso, bits, linesize * sizeT.cy);
			}
			else
			{
				for (INT y = 0; y < sizeT.cy; y++)
				{
					memcpy(bitso + (UINT)y * linesize, bits + (UINT)y * stride, rowcopy);
				}
			}
			DX11Texture2D::Unmap(dx11);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX11Image2D::DrawImage(DX11& dx11)
	{
		if (!Calculate(dx11))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx11.GetImmediateContext()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx11.GetImmediateContext()->IASetIndexBuffer(m_index, DXGI_FORMAT_R32_UINT, 0);
		dx11.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
}