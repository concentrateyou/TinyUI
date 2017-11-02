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
		INT vertexCount = GetVertexCount();
		TinyScopedArray<VERTEXTYPE> vertices(new VERTEXTYPE[vertexCount]);
		ZeroMemory(vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
		//´´½¨¶¥µã»º³å
		HRESULT hRes = dx9.GetD3D()->CreateVertexBuffer(vertexCount, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_vertexBuffer, NULL);
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
	BOOL DX9Image2D::Translate(DX9& dx9, FLOAT ratioX, FLOAT ratioY)
	{
		D3DXVECTOR2 scale(static_cast<FLOAT>(GetScale().cx) * ratioX, static_cast<FLOAT>(GetScale().cy) * ratioY);
		D3DXVECTOR2 pos(static_cast<FLOAT>(GetPosition().x) * ratioX, static_cast<FLOAT>(GetPosition().y) * ratioY);
		VERTEXTYPE vertexs[] =
		{
			{ pos.x,pos.y + scale.y,1,1,0,1 },
			{ pos.x,pos.y,1,1,0,0 },
			{ pos.x + scale.x,pos.y,1,1,1,0 },
			{ pos.x,pos.y + scale.y,1,1,0,1 },
			{ pos.x + scale.x,pos.y,1,1,1,0 },
			{ pos.x + scale.x,pos.y + scale.y,1,1,1,1 }
		};

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
		dx9.GetD3D()->SetTransform(D3DTS_PROJECTION, &ms[0]);
		dx9.GetD3D()->SetTransform(D3DTS_VIEW, &ms[1]);
		dx9.GetD3D()->SetTransform(D3DTS_WORLD, &ms[2]);
		dx9.GetD3D()->SetStreamSource(0, m_vertexBuffer, 0, sizeof(VERTEXTYPE));
		dx9.GetD3D()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		return TRUE;
	}


	void DX9Image2D::Deallocate(DX9& dx9)
	{

	}

}