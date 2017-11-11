#include "stdafx.h"
#include "DX9Font2D.h"

namespace DXFramework
{
	DX9Font2D::DX9Font2D()
	{
	}

	DX9Font2D::~DX9Font2D()
	{
	}

	BOOL DX9Font2D::IsEmpty() const
	{
		return m_d3dFONT == NULL;
	}

	BOOL DX9Font2D::Load(DX9& dx9, HFONT hFONT)
	{
		Destory();
		LOGFONT s = { 0 };
		::GetObject(hFONT, sizeof(LOGFONT), &s);
		D3DXFONT_DESC desc = { 0 };
		desc.CharSet = s.lfCharSet;
		strcpy_s(desc.FaceName, s.lfFaceName);
		desc.Height = s.lfHeight;
		desc.Italic = s.lfItalic;
		desc.OutputPrecision = s.lfOutPrecision;
		desc.PitchAndFamily = s.lfPitchAndFamily;
		desc.Quality = s.lfQuality;
		desc.Weight = s.lfWeight;
		desc.Width = s.lfWidth;
		HRESULT hRes = D3DXCreateFontIndirect(dx9.GetD3D(), &desc, &m_d3dFONT);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	BOOL DX9Font2D::Create(DX9& dx9, D3DXFONT_DESC& desc)
	{
		Destory();
		HRESULT hRes = D3DXCreateFontIndirect(dx9.GetD3D(), &desc, &m_d3dFONT);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	void DX9Font2D::Destory()
	{
		m_d3dFONT.Release();
	}
	INT	 DX9Font2D::MeasureString(DX9& dx9, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color)
	{
		if (dx9.IsEmpty())
			return -1;
		if (IsEmpty())
			return -1;
		return m_d3dFONT->DrawTextA(NULL, pzText, count, pRect, dwFormat, color);
	}
	BOOL DX9Font2D::DrawString(DX9& dx9, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color)
	{
		if (dx9.IsEmpty())
			return FALSE;
		if (IsEmpty())
			return FALSE;
		m_d3dFONT->DrawTextA(NULL, pzText, count, pRect, dwFormat, color);
		return TRUE;
	}
}