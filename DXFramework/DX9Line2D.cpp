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

	BOOL DX9Line2D::DrawLine(DX9& dx9, TinyPoint list[4], D3DCOLOR color)
	{
		if (dx9.IsEmpty())
			return FALSE;
		if (IsEmpty())
			return FALSE;
		D3DXVECTOR2 vertexs[4];
		vertexs[0].x = list[0].x;
		vertexs[0].y = list[0].y;
		vertexs[1].x = list[1].x;
		vertexs[1].y = list[1].y;
		vertexs[2].x = list[2].x;
		vertexs[2].y = list[2].x;
		vertexs[3].x = list[3].x;
		vertexs[3].y = list[3].x;
		HRESULT hRes = m_d3dLINE->Begin();
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dLINE->Draw(vertexs, 4, color);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dLINE->End();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
}