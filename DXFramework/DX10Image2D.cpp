#include "stdafx.h"
#include "DX10Image2D.h"
#include <algorithm>
#include <limits>
#pragma comment(lib,"gdiplus.lib")

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10Image2D, DX10Element2D);

	DX10Image2D::DX10Image2D()
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 6));
	}
	DX10Image2D::~DX10Image2D()
	{

	}
	BOOL DX10Image2D::Initialize(DX10& dx10)
	{
		VERTEXTYPE vertexTypes[6];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 6));
		D3D10_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VERTEXTYPE) * 6;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA	data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertexTypes;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		HRESULT hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_vertex);
		if (hRes != S_OK)
			return FALSE;
		ULONG indices[6];
		ZeroMemory(indices, (sizeof(ULONG) * 6));
		for (INT i = 0; i < 6; i++)
		{
			indices[i] = i;
		}
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ULONG) * 6;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hRes = dx10.GetD3D()->CreateBuffer(&desc, &data, &m_index);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX10Image2D::Calculate(DX10& dx10)
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
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		UINT count = 1;
		dx10.GetD3D()->RSGetViewports(&count, &vp);
		XMFLOAT2 size(m_size.x * m_scale.x, m_size.y * m_scale.y);
		XMFLOAT2 center(static_cast<FLOAT>(vp.Width) / 2, static_cast<FLOAT>(vp.Height) / 2);
		XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, 0.0F);
		XMMATRIX rotation = XMMatrixRotationZ(((m_angle) * (D3DX_PI / 180.0)));
		XMMATRIX translation = XMMatrixTranslation(-center.x + size.x / 2 + m_translate.x, center.y - size.y / 2 - m_translate.y, 0.0F);
		XMMATRIX val = scaling * rotation * translation;
		XMMATRIX* ms = dx10.GetMatrixs();
		ms[1] *= val;
		BYTE* bits = NULL;
		HRESULT hRes = m_vertex->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, (void*)m_vertexTypes, sizeof(VERTEXTYPE) * 6);
		m_vertex->Unmap();
		return TRUE;
	}
	BOOL DX10Image2D::Create(DX10& dx10, D3D10_TEXTURE2D_DESC& desc)
	{
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Create(dx10, desc))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Create(DX10& dx10, ID3D10Texture2D* texture2D)
	{
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Create(dx10, texture2D))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Create(DX10& dx10, INT cx, INT cy, DXGI_FORMAT dxgiFormat, const BYTE* bits, BOOL bReadoly)
	{
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Create(dx10, cx, cy, dxgiFormat, bits, bReadoly))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Create(DX10& dx10, INT cx, INT cy, DXGI_FORMAT dxgiFormat)
	{
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Create(dx10, cx, cy, dxgiFormat))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Load(DX10& dx10, const BYTE* bits, LONG size)
	{
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Load(dx10, bits, size))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Load(DX10& dx10, HANDLE hResource)
	{
		if (!hResource)
			return FALSE;
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Load(dx10, hResource))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::Load(DX10& dx10, const CHAR* pzFile)
	{
		if (!PathFileExists(pzFile))
			return FALSE;
		Destory();
		if (!Initialize(dx10))
			return FALSE;
		if (DX10Texture2D::Load(dx10, pzFile))
		{
			D3D10_TEXTURE2D_DESC desc;
			m_texture2D->GetDesc(&desc);
			m_size.x = static_cast<FLOAT>(desc.Width);
			m_size.y = static_cast<FLOAT>(desc.Height);
			return TRUE;
		}
		return FALSE;
	}
	void DX10Image2D::Destory()
	{
		m_vertex.Release();
		m_index.Release();
		DX10Texture2D::Destory();
	}
	BOOL DX10Image2D::BitBlt(DX10& dx10, const BYTE* bits, LONG size, LONG linesize)
	{
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		if (!bits || DX10Texture2D::IsEmpty() || size != (sizeT.cx * sizeT.cy * 4))
			return FALSE;
		HDC hDC = NULL;
		if (!DX10Texture2D::GetDC(FALSE, hDC))
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
		return DX10Texture2D::ReleaseDC();
	}
	BOOL DX10Image2D::BitBlt(DX10& dx10, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src)
	{
		if (DX10Texture2D::IsEmpty())
			return FALSE;
		HDC hDC = NULL;
		if (!DX10Texture2D::GetDC(TRUE, hDC))
			return FALSE;
		TinyFramework::TinyMemDC mdc(hDC, hBitmapSrc);
		::BitBlt(hDC, dst.left, dst.top, dst.Width(), dst.Height(), mdc, src.x, src.y, SRCCOPY | CAPTUREBLT);
		return DX10Texture2D::ReleaseDC();
	}
	BOOL DX10Image2D::BitBlt(DX10& dx10, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src)
	{
		if (DX10Texture2D::IsEmpty())
			return FALSE;
		HDC hDC = NULL;
		if (!DX10Texture2D::GetDC(TRUE, hDC))
			return FALSE;
		::BitBlt(hDC, dst.left, dst.top, dst.Width(), dst.Height(), hDCSrc, src.x, src.y, SRCCOPY | CAPTUREBLT);
		return DX10Texture2D::ReleaseDC();
	}
	BOOL DX10Image2D::Copy(DX10& dx10, D3D10_BOX* ps, const BYTE* bits, LONG size)
	{
		if (DX10Texture2D::IsEmpty() || !bits || size <= 0)
			return FALSE;
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		return DX10Texture2D::Copy(dx10, ps, bits, size, sizeT.cx * 4, sizeT.cx * sizeT.cy * 4);
	}
	BOOL DX10Image2D::Copy(DX10& dx10, const BYTE* bits, LONG size, UINT stride)
	{
		if (DX10Texture2D::IsEmpty() || !bits || size <= 0)
			return FALSE;
		TinySize sizeT(static_cast<LONG>(m_size.x), static_cast<LONG>(m_size.y));
		BYTE* bitso = NULL;
		UINT linesize = 0;
		UINT rowcopy = (stride < linesize) ? stride : linesize;
		D3D10_MAPPED_TEXTURE2D ms;
		if (DX10Texture2D::Map(ms))
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
			DX10Texture2D::Unmap();
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX10Image2D::DrawImage(DX10& dx10)
	{
		if (!Calculate(dx10))
			return FALSE;
		UINT stride = sizeof(VERTEXTYPE);
		UINT offset = 0;
		dx10.GetD3D()->IASetVertexBuffers(0, 1, &m_vertex, &stride, &offset);
		dx10.GetD3D()->IASetIndexBuffer(m_index, DXGI_FORMAT_R32_UINT, 0);
		dx10.GetD3D()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return TRUE;
	}
}