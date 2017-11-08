#include "stdafx.h"
#include "DX9Image2D.h"
#include <algorithm>
#include <limits>

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX9Image2D, DX9Element2D);

	DX9Image2D::DX9Image2D()
	{

	}

	DX9Image2D::~DX9Image2D()
	{

	}

	BOOL DX9Image2D::Initialize(DX9& dx9)
	{
		m_vertexBuffer.Release();
		INT vertexCount = GetVertexCount();
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[vertexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
		//´´½¨¶¥µã»º³å
		HRESULT hRes = dx9.GetD3D()->CreateVertexBuffer(sizeof(VERTEXTYPE) * vertexCount, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL);
		if (hRes != S_OK)
			return FALSE;
		void* bits = NULL;
		hRes = m_vertexBuffer->Lock(0, 0, &bits, 0);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
		hRes = m_vertexBuffer->Unlock();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb219690(v=vs.85).aspx
	BOOL DX9Image2D::Translate(DX9& dx9, FLOAT ratioX, FLOAT ratioY)
	{
		D3DXVECTOR2 scale(static_cast<FLOAT>(GetScale().cx) * ratioX, static_cast<FLOAT>(GetScale().cy) * ratioY);
		D3DXVECTOR2 pos(static_cast<FLOAT>(GetPosition().x) * ratioX, static_cast<FLOAT>(GetPosition().y) * ratioY);
		VERTEXTYPE vertexs[] =
		{
			{ static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y + scale.y),1.0F,1.0F,0.0F,1.0F },
			{ static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),1.0F,1.0F,0.0F,0.0F },
			{ static_cast<FLOAT>(pos.x + scale.x),static_cast<FLOAT>(pos.y),1.0F,1.0,1.0F,0.0F },
			{ static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y + scale.y),1.0F,1.0F,0.0F,1.0F },
			{ static_cast<FLOAT>(pos.x + scale.x),static_cast<FLOAT>(pos.y),1.0F,1.0F,1.0F,0.0F },
			{ static_cast<FLOAT>(pos.x + scale.x),static_cast<FLOAT>(pos.y + scale.y),1.0F,1.0F,1.0F,1.0F }
		};
		for (INT i = 0; i < 6; i++)
		{
			vertexs[i].x -= 0.5F;
			vertexs[i].y -= 0.5F;
		}
		BYTE* bits = NULL;
		HRESULT hRes = m_vertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&bits), 0);
		if (hRes != S_OK)
			return FALSE;
		INT vertexCount = GetVertexCount();
		memcpy(bits, vertexs, (sizeof(VERTEXTYPE) * vertexCount));
		hRes = m_vertexBuffer->Unlock();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	BOOL DX9Image2D::Load(DX9& dx9, const CHAR* pzFile)
	{
		if (!Initialize(dx9))
			return FALSE;
		TinySize texture2DSize;
		if (!DX9Texture2D::Load(dx9, pzFile, texture2DSize))
			return FALSE;
		SetSize(texture2DSize);
		SetScale(texture2DSize);
		return TRUE;
	}

	BOOL DX9Image2D::Load(DX9& dx9, const BYTE* bits, LONG size)
	{
		if (!Initialize(dx9))
			return FALSE;
		TinySize texture2DSize;
		if (!DX9Texture2D::Load(dx9, bits, size, texture2DSize))
			return FALSE;
		SetSize(texture2DSize);
		SetScale(texture2DSize);
		return TRUE;
	}

	BOOL DX9Image2D::Create(DX9& dx9, INT cx, INT cy, const BYTE* bits)
	{
		if (!Initialize(dx9))
			return FALSE;
		if (!DX9Texture2D::Create(dx9, cx, cy, bits))
			return FALSE;
		TinySize size(cx, cy);
		SetSize(size);
		SetScale(size);
		return TRUE;
	}

	INT	DX9Image2D::GetVertexCount() const
	{
		return 6;
	}
	BOOL DX9Image2D::Allocate(DX9& dx9)
	{
		return TRUE;
	}

	BOOL DX9Image2D::Process(DX9& dx9)
	{
		if (dx9.IsEmpty())
			return FALSE;
		D3DXMATRIX* ms = dx9.GetMatrixs();
		dx9.GetD3D()->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_texture2D.Ptr()));
		dx9.GetD3D()->SetTransform(D3DTS_PROJECTION, &ms[0]);
		dx9.GetD3D()->SetTransform(D3DTS_VIEW, &ms[1]);
		dx9.GetD3D()->SetTransform(D3DTS_WORLD, &ms[2]);
		dx9.GetD3D()->SetStreamSource(0, m_vertexBuffer, 0, sizeof(VERTEXTYPE));
		dx9.GetD3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		dx9.GetD3D()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		return TRUE;
	}

	void DX9Image2D::Deallocate(DX9& dx9)
	{

	}
}