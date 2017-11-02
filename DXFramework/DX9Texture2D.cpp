#include "stdafx.h"
#include "DX9Texture2D.h"

namespace DXFramework
{
	DX9Texture2D::DX9Texture2D()
	{
	}

	DX9Texture2D::~DX9Texture2D()
	{
	}
	BOOL DX9Texture2D::Load(DX9& dx9, const CHAR* pzFile, TinySize& texture2DSize)
	{
		m_texture2D.Release();
		D3DXIMAGE_INFO sINFO;
		HRESULT hRes = D3DXCreateTextureFromFileEx(dx9.GetD3D(), pzFile, 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &sINFO, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		texture2DSize.cx = sINFO.Width;
		texture2DSize.cy = sINFO.Height;
		return TRUE;
	}
	BOOL DX9Texture2D::Load(DX9& dx9, const BYTE* bits, LONG size, TinySize& texture2DSize)
	{
		m_texture2D.Release();
		D3DXIMAGE_INFO sINFO;
		HRESULT hRes = D3DXCreateTextureFromFileInMemoryEx(dx9.GetD3D(), static_cast<LPCVOID>(bits), static_cast<UINT>(size), 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &sINFO, NULL, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		texture2DSize.cx = sINFO.Width;
		texture2DSize.cy = sINFO.Height;
		return TRUE;
	}
	BOOL DX9Texture2D::Create(DX9& dx9, INT cx, INT cy, const BYTE* bits)
	{
		m_texture2D.Release();
		HRESULT hRes = dx9.GetD3D()->CreateTexture(cx, cy, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture2D, NULL);
		if (hRes != S_OK)
			return FALSE;
		if (bits != NULL)
		{
			D3DLOCKED_RECT lockRect = { 0 };
			hRes = m_texture2D->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
			if (hRes != S_OK)
				return FALSE;
			memcpy(lockRect.pBits, bits, cx * 4);
			hRes = m_texture2D->UnlockRect(0);
			if (hRes != S_OK)
				return FALSE;
		}
		return TRUE;
	}
	BOOL DX9Texture2D::IsEmpty() const
	{
		return m_texture2D == NULL;
	}
}