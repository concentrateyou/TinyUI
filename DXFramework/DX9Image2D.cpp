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
	BOOL DX9Image2D::Translate(DX9& dx9, FLOAT ratioX, FLOAT ratioY, FLOAT rotate, BOOL bFlipH, BOOL bFlipV)
	{
		INT vertexCount = GetVertexCount();
		D3DXVECTOR2 scale(static_cast<FLOAT>(GetScale().cx) * ratioX, static_cast<FLOAT>(GetScale().cy) * ratioY);
		D3DXVECTOR2 pos(static_cast<FLOAT>(GetPosition().x) * ratioX, static_cast<FLOAT>(GetPosition().y) * ratioY);
		D3DXVECTOR2 center(pos.x + scale.x / 2, pos.y + scale.y / 2);
		VERTEXTYPE vertices[] =
		{
			{ pos.x, pos.y,  0.0F, 1.0F,(bFlipH ? -0.0F : 0.0F),(bFlipV ? -0.0F : 0.0F) },
			{ pos.x + scale.x, pos.y,  0.0F, 1.0F, (bFlipH ? -1.0F : 1.0F),(bFlipV ? -0.0F : 0.0F) },
			{ pos.x, pos.y + scale.y, 0.0F, 1.0F, (bFlipH ? -0.0F : 0.0F), (bFlipV ? -1.0F : 1.0F) },
			{ pos.x + scale.x, pos.y + scale.y, 0.0F, 1.0F,(bFlipH ? -1.0F : 1.0F), (bFlipV ? -1.0F : 1.0F) },
		};

		if (rotate != 0.0F)
		{
			//https://math.stackexchange.com/questions/270194/how-to-find-the-vertices-angle-after-rotation
			rotate = D3DXToRadian(rotate);
			D3DXVECTOR2 pos1((pos.x - scale.x) / 2, (pos.y - scale.y) / 2);
			D3DXVECTOR2 pos2((scale.x - pos.x) / 2, (scale.y - pos.y) / 2);
			vertices[0].x = pos1.x * cosf(rotate) - pos1.y * sinf(rotate);
			vertices[0].y = pos1.x * sinf(rotate) + pos1.y * cosf(rotate);
			vertices[1].x = pos2.x * cosf(rotate) - pos1.y * sinf(rotate);
			vertices[1].y = pos2.x * sinf(rotate) + pos1.y * cosf(rotate);
			vertices[2].x = pos1.x * cosf(rotate) - pos2.y * sinf(rotate);
			vertices[2].y = pos1.x * sinf(rotate) + pos2.y * cosf(rotate);
			vertices[3].x = pos2.x * cosf(rotate) - pos2.y * sinf(rotate);
			vertices[3].y = pos2.x * sinf(rotate) + pos2.y * cosf(rotate);
			vertices[0].x += center.x;
			vertices[0].y += center.y;
			vertices[1].x += center.x;
			vertices[1].y += center.y;
			vertices[2].x += center.x;
			vertices[2].y += center.y;
			vertices[3].x += center.x;
			vertices[3].y += center.y;
		}
		for (INT i = 0; i < vertexCount; i++)
		{
			vertices[i].x -= 0.5F;
			vertices[i].y -= 0.5F;
		}

		BYTE* bits = NULL;
		HRESULT hRes = m_vertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&bits), 0);
		if (hRes != S_OK)
		{
			TRACE("Translate Lock:%d\n", hRes);
			return FALSE;
		}
		memcpy(bits, vertices, (sizeof(VERTEXTYPE) * vertexCount));
		hRes = m_vertexBuffer->Unlock();
		if (hRes != S_OK)
		{
			TRACE("Translate Unlock:%d\n", hRes);
			return FALSE;
		}
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
		return 4;
	}
	BOOL DX9Image2D::Allocate(DX9& dx9)
	{
		return TRUE;
	}

	BOOL DX9Image2D::Process(DX9& dx9)
	{
		if (dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = dx9.GetD3D()->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_texture2D.Ptr()));
		if (hRes != S_OK)
		{
			TRACE("[Process] SetTexture:%d\n", hRes);
			LOG(ERROR) << "[Process] SetTexture:" << hRes;
			return FALSE;
		}
		hRes = dx9.GetD3D()->SetStreamSource(0, m_vertexBuffer, 0, sizeof(VERTEXTYPE));
		if (hRes != S_OK)
		{
			TRACE("[Process] SetStreamSource:%d\n", hRes);
			LOG(ERROR) << "[Process] SetStreamSource:" << hRes;
			return FALSE;
		}
		hRes = dx9.GetD3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		if (hRes != S_OK)
		{
			TRACE("[Process] SetSamplerState:%d\n", hRes);
			LOG(ERROR) << "[Process] SetSamplerState:" << hRes;
			return FALSE;
		}
		hRes = dx9.GetD3D()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		if (hRes != S_OK)
		{
			TRACE("[Process] DrawPrimitive:%d\n", hRes);
			LOG(ERROR) << "[Process] DrawPrimitive:" << hRes;
			return FALSE;
		}
		return TRUE;
	}

	void DX9Image2D::Deallocate(DX9& dx9)
	{

	}
}