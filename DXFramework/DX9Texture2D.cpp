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
	BOOL DX9Texture2D::Load(DX9& dx9, const CHAR* pzFile)
	{
		m_texture2D.Release();
		HRESULT hRes = D3DXCreateTextureFromFile(dx9.GetD3D(), pzFile, &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX9Texture2D::Load(DX9& dx9, const BYTE* bits, LONG size)
	{
		m_texture2D.Release();
		HRESULT hRes = D3DXCreateTextureFromFileInMemory(dx9.GetD3D(), static_cast<LPCVOID>(bits), static_cast<UINT>(size), &m_texture2D);
		if (hRes != S_OK)
			return FALSE;
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