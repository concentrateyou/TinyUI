#include "stdafx.h"
#include "DX9Line2D.h"

namespace DXFramework
{
	DX9Line2D::DX9Line2D()
	{
	}

	DX9Line2D::~DX9Line2D()
	{
	}

	BOOL DX9Line2D::IsEmpty() const
	{
		return m_d3dLINE == NULL;
	}

	BOOL DX9Line2D::Create(DX9& dx9)
	{
		Destory();
		HRESULT hRes = D3DXCreateLine(dx9.GetD3D(), &m_d3dLINE);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	void DX9Line2D::Destory()
	{
		m_d3dLINE.Release();
	}

	BOOL DX9Line2D::DrawLine(DX9& dx9, D3DXVECTOR2 list[4], D3DCOLOR color)
	{
		if (dx9.IsEmpty())
			return FALSE;
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_d3dLINE->Begin();
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dLINE->Draw(list, 4, color);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dLINE->End();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
}