#include "../stdafx.h"
#include "TinyDDraw.h"

namespace TinyFramework
{
	TinyImageSurface::TinyImageSurface()
	{

	}
	BOOL TinyImageSurface::Create(LPDIRECTDRAW7 pDD7, INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
		SAFE_RELEASE(m_dds);
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = cx;
		ddsd.dwHeight = cy;
		HRESULT hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
		if (hRes != DD_OK)
		{
			if (hRes == DDERR_OUTOFVIDEOMEMORY)
			{
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
				hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
			}
			if (hRes != DD_OK)
				return FALSE;
		}
		return TRUE;
	}
	BOOL TinyImageSurface::LoadImage(LPDIRECTDRAW7 pDD7, LPCSTR pzFile)
	{
		ASSERT(pDD7);
		SAFE_RELEASE(m_dds);
		if (!m_image.Open(pzFile))
			return FALSE;
		m_size = m_image.GetSize();
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = m_size.cx;
		ddsd.dwHeight = m_size.cy;
		HRESULT hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
		if (hRes != DD_OK)
		{
			if (hRes == DDERR_OUTOFVIDEOMEMORY)
			{
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
				hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
			}
			if (hRes != DD_OK)
				return FALSE;
		}
		HDC hMenDC = CreateCompatibleDC(NULL);
		if (!hMenDC)
			return FALSE;
		HGDIOBJ hObj = SelectObject(hMenDC, m_image.GetHBITMAP());
		HDC hDC = NULL;
		if ((hRes = m_dds->GetDC(&hDC)) == DD_OK)
		{
			BitBlt(hDC, 0, 0, m_size.cx, m_size.cy, hMenDC, 0, 0, SRCCOPY);
			m_dds->ReleaseDC(hDC);
		}
		SelectObject(hMenDC, hObj);
		DeleteDC(hMenDC);
		return TRUE;
	}
	BOOL TinyImageSurface::LoadImage(LPDIRECTDRAW7 pDD7, BYTE* ps, DWORD size)
	{
		ASSERT(pDD7);
		SAFE_RELEASE(m_dds);
		if (!m_image.Open(ps, size))
			return FALSE;
		m_size = m_image.GetSize();
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = m_size.cx;
		ddsd.dwHeight = m_size.cy;
		HRESULT hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
		if (hRes != DD_OK)
		{
			if (hRes == DDERR_OUTOFVIDEOMEMORY)
			{
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
				hRes = pDD7->CreateSurface(&ddsd, &m_dds, NULL);
			}
			if (hRes != DD_OK)
				return FALSE;
		}
		HDC hMenDC = CreateCompatibleDC(NULL);
		if (!hMenDC)
			return FALSE;
		HGDIOBJ hObj = SelectObject(hMenDC, m_image.GetHBITMAP());
		HDC hDC = NULL;
		if ((hRes = m_dds->GetDC(&hDC)) == DD_OK)
		{
			BitBlt(hDC, 0, 0, m_size.cx, m_size.cy, hMenDC, 0, 0, SRCCOPY);
			m_dds->ReleaseDC(hDC);
		}
		SelectObject(hMenDC, hObj);
		DeleteDC(hMenDC);
		return TRUE;
	}
	BOOL TinyImageSurface::Fill(LPVOID ps, DWORD dwSize)
	{
		HRESULT hRes;
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		m_dds->Restore();
		hRes = m_dds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		if (hRes != DD_OK)
			return FALSE;
		memcpy(ddsd.lpSurface, ps, dwSize);
		return m_dds->Unlock(NULL) == DD_OK;
	}
	BOOL TinyImageSurface::Draw(LPDIRECTDRAWSURFACE7 lpDest, INT destX, INT destY, INT srcX, INT srcY, INT srcCX, INT srcCY)
	{
		if (srcCX == 0)
			srcCX = m_size.cx;
		if (srcCY == 0)
			srcCY = m_size.cy;
		RECT	rcRect;
		rcRect.left = srcX;
		rcRect.top = srcY;
		rcRect.right = srcCX + srcX;
		rcRect.bottom = srcCY + srcY;
		HRESULT	hRes;
		while (1)
		{
			hRes = lpDest->BltFast(destX, destY, m_dds, &rcRect, DDBLTFAST_SRCCOLORKEY);
			if (hRes == DD_OK)
				break;

			if (hRes == DDERR_SURFACELOST)
			{
				m_dds->Restore();
			}
			else
			{
				if (hRes != DDERR_WASSTILLDRAWING)
					return FALSE;
			}
		}
		return TRUE;
	}
	LPDIRECTDRAWSURFACE7 TinyImageSurface::GetSurface() const
	{
		return m_dds;
	}
	//////////////////////////////////////////////////////////////////////////
	TinySurface::TinySurface()
	{

	}
	BOOL TinySurface::Create()
	{
		HRESULT hRes = DirectDrawCreateEx(NULL, (VOID**)&m_dd7, IID_IDirectDraw7, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dd7->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		if (hRes != S_OK)
			return FALSE;
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		hRes = m_dd7->CreateSurface(&ddsd, &m_ddsPrimary, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void TinySurface::Render()
	{
		HRESULT hRes = S_OK;
		while (1)
		{
			hRes = m_ddsPrimary->Flip(NULL, 0);
			if (hRes == DD_OK)
				break;
			if (hRes == DDERR_SURFACELOST)
				m_ddsPrimary->Restore();
			if (hRes != DDERR_WASSTILLDRAWING)
				break;
		}
	}
	LPDIRECTDRAW7 TinySurface::GetDirectDraw() const
	{
		return m_dd7;
	}
	TinySurface::~TinySurface()
	{

	}
};
