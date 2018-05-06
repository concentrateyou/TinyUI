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
		Destory();
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
		Destory();
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
		Destory();
		HRESULT hRes = dx9.GetD3D()->CreateTexture(cx, cy, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture2D, NULL);
		if (hRes != S_OK)
		{
			TRACE("DX9Texture2D CreateTexture:%d\n", hRes);
			return FALSE;
		}
		if (bits != NULL)
		{
			D3DLOCKED_RECT lockRect = { 0 };
			hRes = m_texture2D->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
			if (hRes != S_OK)
			{
				TRACE("[Create] LockRect:%d\n", hRes);
				LOG(ERROR) << "[Create] LockRect::" << hRes;
				return FALSE;
			}
			memcpy(lockRect.pBits, bits, cx * cy * 4);
			hRes = m_texture2D->UnlockRect(0);
			if (hRes != S_OK)
			{
				TRACE("[Create] UnlockRect:%d\n", hRes);
				LOG(ERROR) << "[Create] UnlockRect::" << hRes;
				return FALSE;
			}
		}
		return TRUE;
	}
	void DX9Texture2D::Destory()
	{
		m_texture2D.Release();
	}
	BOOL DX9Texture2D::SaveAs(const CHAR* pzFile, D3DXIMAGE_FILEFORMAT dxgi)
	{
		if (IsEmpty())
			return FALSE;
		if (!pzFile)
			return FALSE;
		return SUCCEEDED(D3DXSaveTextureToFile(pzFile, dxgi, m_texture2D, NULL));
	}
	BOOL DX9Texture2D::Copy(BYTE* bits, LONG size)
	{
		if (IsEmpty())
			return FALSE;
		if (!bits)
			return FALSE;
		D3DLOCKED_RECT lockRect = { 0 };
		HRESULT hRes = m_texture2D->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
		if (hRes != S_OK)
		{
			TRACE("[Copy] LockRect:%d\n", hRes);
			LOG(ERROR) << "[Copy] LockRect::" << hRes;
			return FALSE;
		}
		memcpy((BYTE*)lockRect.pBits, bits, size);
		hRes = m_texture2D->UnlockRect(0);
		if (hRes != S_OK)
		{
			TRACE("[Copy] UnlockRect:%d\n", hRes);
			LOG(ERROR) << "[Copy] UnlockRect::" << hRes;
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX9Texture2D::Copy(BYTE* bits, INT linesize, INT cy)
	{
		if (IsEmpty())
			return FALSE;
		if (!bits)
			return FALSE;
		D3DSURFACE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		m_texture2D->GetLevelDesc(0, &desc);
		ASSERT(desc.Height == cy);
		D3DLOCKED_RECT lockRect = { 0 };
		HRESULT hRes = m_texture2D->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
		if (hRes != S_OK)
		{
			TRACE("[Copy] LockRect:%d\n", hRes);
			LOG(ERROR) << "[Copy] LockRect::" << hRes;
			return FALSE;
		}
		for (INT i = 0; i < cy; i++)
		{
			memcpy((BYTE*)lockRect.pBits + i* lockRect.Pitch, bits + i * linesize, linesize);
		}
		hRes = m_texture2D->UnlockRect(0);
		if (hRes != S_OK)
		{
			TRACE("[Copy] UnlockRect:%d\n", hRes);
			LOG(ERROR) << "[Copy] UnlockRect::" << hRes;
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX9Texture2D::IsEmpty() const
	{
		return m_texture2D == NULL;
	}
}