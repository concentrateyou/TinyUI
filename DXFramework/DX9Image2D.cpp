#include "stdafx.h"
#include "DX9Image2D.h"
#include <algorithm>
#include <limits>

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX9Image2D, DX9Element2D);

	DX9Image2D::DX9Image2D()
		:m_bFlipV(FALSE),
		m_bFlipH(FALSE),
		m_angle(0.0F)
	{
		m_scale.x = m_scale.y = 1.0F;
	}

	DX9Image2D::~DX9Image2D()
	{

	}

	BOOL DX9Image2D::Initialize(DX9& dx9)
	{
		m_vertexs.Release();
		//´´½¨¶¥µã»º³å
		HRESULT hRes = dx9.GetD3D()->CreateVertexBuffer(sizeof(VERTEXTYPE) * 4, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_vertexs, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb219690(v=vs.85).aspx
	BOOL DX9Image2D::Translate(DX9& dx9)
	{
		D3DXVECTOR2 size(static_cast<FLOAT>(m_trackerRect.Size().cx) * m_scale.x, static_cast<FLOAT>(m_trackerRect.Size().cy) * m_scale.y);
		D3DXVECTOR2 pos(static_cast<FLOAT>(m_trackerRect.Position().x) * m_scale.x, static_cast<FLOAT>(m_trackerRect.Position().y) * m_scale.y);
		D3DXVECTOR2 center(pos.x + size.x / 2, pos.y + size.y / 2);
		VERTEXTYPE vertices[] =
		{
			{ pos.x, pos.y,  0.0F, 1.0F,0xFFFFFFFF, (m_bFlipH ? -0.0F : 0.0F),(m_bFlipV ? -0.0F : 0.0F) },
			{ pos.x + size.x, pos.y,  0.0F, 1.0F,0xFFFFFFFF, (m_bFlipH ? -1.0F : 1.0F),(m_bFlipV ? -0.0F : 0.0F) },
			{ pos.x, pos.y + size.y, 0.0F, 1.0F,0xFFFFFFFF, (m_bFlipH ? -0.0F : 0.0F), (m_bFlipV ? -1.0F : 1.0F) },
			{ pos.x + size.x, pos.y + size.y, 0.0F, 1.0F,0xFFFFFFFF,(m_bFlipH ? -1.0F : 1.0F), (m_bFlipV ? -1.0F : 1.0F) },
		};
		if (m_angle != 0.0F)
		{
			//https://math.stackexchange.com/questions/270194/how-to-find-the-vertices-angle-after-rotation
			FLOAT rotate = D3DXToRadian(m_angle);
			D3DXVECTOR2 pos1((pos.x - size.x) / 2, (pos.y - size.y) / 2);
			D3DXVECTOR2 pos2((size.x - pos.x) / 2, (size.y - pos.y) / 2);
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
		for (INT i = 0; i < 4; i++)
		{
			vertices[i].x -= 0.5F;
			vertices[i].y -= 0.5F;
		}
		BYTE* bits = NULL;
		HRESULT hRes = m_vertexs->Lock(0, 0, reinterpret_cast<void**>(&bits), 0);
		if (hRes != S_OK)
		{
			TRACE("Translate Lock:%d\n", hRes);
			return FALSE;
		}
		memcpy(bits, vertices, (sizeof(VERTEXTYPE) * 4));
		hRes = m_vertexs->Unlock();
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
		if (!DX9Texture2D::Load(dx9, pzFile, m_size))
			return FALSE;
		SetRectangle(TinyPoint(), m_size);
		return TRUE;
	}

	BOOL DX9Image2D::Load(DX9& dx9, const BYTE* bits, LONG size)
	{
		if (!Initialize(dx9))
			return FALSE;
		if (!DX9Texture2D::Load(dx9, bits, size, m_size))
			return FALSE;
		SetRectangle(TinyPoint(), m_size);
		return TRUE;
	}

	BOOL DX9Image2D::Create(DX9& dx9, INT cx, INT cy, const BYTE* bits)
	{
		if (!Initialize(dx9))
			return FALSE;
		if (!DX9Texture2D::Create(dx9, cx, cy, bits))
			return FALSE;
		m_size.SetSize(cx, cy);
		SetRectangle(TinyPoint(), m_size);
		return TRUE;
	}

	void DX9Image2D::SetRotate(FLOAT angle)
	{
		m_angle = angle;
	}
	void DX9Image2D::SetScale(const D3DXVECTOR2& scale)
	{
		m_scale = scale;
	}
	void DX9Image2D::SetTranslate(const D3DXVECTOR2& pos)
	{
		m_pos = pos;
	}
	void DX9Image2D::SetFlipH(BOOL bFlag)
	{
		m_bFlipH = bFlag;
	}
	void DX9Image2D::SetFlipV(BOOL bFlag)
	{
		m_bFlipV = bFlag;
	}

	BOOL DX9Image2D::Allocate(DX9& dx9)
	{
		return TRUE;
	}

	BOOL DX9Image2D::Process(DX9& dx9)
	{
		if (dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = dx9.GetD3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		if (hRes != S_OK)
		{
			TRACE("[Process] SetFVF:%d\n", hRes);
			LOG(ERROR) << "[Process] SetFVF::" << hRes;
			return FALSE;
		}
		hRes = dx9.GetD3D()->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_texture2D.Ptr()));
		if (hRes != S_OK)
		{
			TRACE("[Process] SetTexture:%d\n", hRes);
			LOG(ERROR) << "[Process] SetTexture:" << hRes;
			return FALSE;
		}
		hRes = dx9.GetD3D()->SetStreamSource(0, m_vertexs, 0, sizeof(VERTEXTYPE));
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