#include "../stdafx.h"
#include "TinyDDraw.h"

namespace TinyUI
{
	TinySurface::TinySurface()
	{

	}
	BOOL TinySurface::Initialize(INT cx, INT cy)
	{
		HRESULT hRes = DirectDrawCreateEx(NULL, (VOID**)&m_dd7, IID_IDirectDraw7, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_dd7->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_dd7->SetDisplayMode(cx, cy, 24, 0, 0);
		if (FAILED(hRes))
			return FALSE;
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
		hRes = m_dd7->CreateSurface(&ddsd, &m_ddsPrimary, NULL);
		if (FAILED(hRes))
			return FALSE;
		DDSCAPS2  ddscaps;
		ZeroMemory(&ddscaps, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		hRes = m_ddsPrimary->GetAttachedSurface(&ddscaps, &m_ddsBack);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL TinySurface::Render()
	{

	}
	TinySurface::~TinySurface()
	{

	}
};
