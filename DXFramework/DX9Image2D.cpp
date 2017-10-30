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
		HRESULT hRes = dx9.GetD3D()->CreateVertexBuffer(vertexCount, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_vertexBuffer, NULL);
		if (hRes != S_OK)
			return FALSE;
		UINT sizeToLock = 0;
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
	BOOL DX9Image2D::Transform(DX9& dx9, FLOAT ratioX, FLOAT ratioY)
	{
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
		return TRUE;
	}


	void DX9Image2D::Deallocate(DX9& dx9)
	{

	}

}