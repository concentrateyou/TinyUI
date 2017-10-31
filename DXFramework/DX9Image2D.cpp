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
		m_vertices.Reset(new VERTEXTYPE[vertexCount]);
		return TRUE;
	}
	BOOL DX9Image2D::IsEmpty() const
	{
		return m_texture.IsEmpty();
	}
	BOOL DX9Image2D::Transform(DX9& dx9, FLOAT ratioX, FLOAT ratioY)
	{
		FLOAT left = 0.0F;
		FLOAT right = 0.0F;
		FLOAT top = 0.0F;
		FLOAT bottom = 0.0F;
		D3DVIEWPORT9 vp;
		ZeroMemory(&vp, sizeof(vp));
		dx9.GetD3D()->GetViewport(&vp);
		XMFLOAT2 scale(static_cast<FLOAT>(GetScale().cx) * ratioX, static_cast<FLOAT>(GetScale().cy) * ratioY);
		XMFLOAT2 pos(static_cast<FLOAT>(GetPosition().x) * ratioX, static_cast<FLOAT>(GetPosition().y) * ratioY);
		XMFLOAT2 size(vp.Width, vp.Height);
		left = (FLOAT)((size.x / 2) * -1) + pos.x;
		right = left + scale.x;
		top = (FLOAT)(size.y / 2) - pos.y;
		bottom = top - scale.y;
		INT vertexCount = GetVertexCount();
		m_vertices[0].position = XMFLOAT4(left, top, 1.0F, 1.0F);
		m_vertices[0].texture = XMFLOAT2(0.0F, 0.0F);
		m_vertices[1].position = XMFLOAT4(right, bottom, 1.0F, 1.0F);
		m_vertices[1].texture = XMFLOAT2(1.0F, 1.0f);
		m_vertices[2].position = XMFLOAT4(left, bottom, 1.0F, 1.0F);
		m_vertices[2].texture = XMFLOAT2(0.0F, 1.0f);
		m_vertices[3].position = XMFLOAT4(left, top, 1.0F, 1.0F);
		m_vertices[3].texture = XMFLOAT2(0.0F, 0.0F);
		m_vertices[4].position = XMFLOAT4(right, top, 1.0F, 1.0F);
		m_vertices[4].texture = XMFLOAT2(1.0F, 0.0F);
		m_vertices[5].position = XMFLOAT4(right, bottom, 1.0F, 1.0F);
		m_vertices[5].texture = XMFLOAT2(1.0F, 1.0F);
		BYTE* bits = NULL;
		HRESULT hRes = m_vertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&bits), 0);
		if (hRes != S_OK)
			return FALSE;
		memcpy(bits, m_vertices.Ptr(), (sizeof(VERTEXTYPE) * vertexCount));
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
		dx9.GetD3D()->SetStreamSource(0, m_vertexBuffer, 0, sizeof(VERTEXTYPE));
		dx9.GetD3D()->SetRenderState(D3DRS_LIGHTING, FALSE);
		D3DXMATRIX* ms = dx9.GetMatrixs();
		dx9.GetD3D()->SetTransform(D3DTS_VIEW, &ms[1]);
		dx9.GetD3D()->SetTransform(D3DTS_WORLD, &ms[1]);
		dx9.GetD3D()->SetTransform(D3DTS_PROJECTION, &ms[2]);
		return TRUE;
	}


	void DX9Image2D::Deallocate(DX9& dx9)
	{

	}

}