#pragma once
#include "DX9Element2D.h"
#include "DX9Texture2D.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX9Image2D : public DX9Element2D, public DX9Texture2D
	{
		struct VERTEXTYPE
		{
			FLOAT x, y, z, rhw;
			FLOAT u, v;
		};
		DECLARE_DYNAMIC(DX9Image2D)
		DISALLOW_COPY_AND_ASSIGN(DX9Image2D)
	public:
		DX9Image2D();
		virtual ~DX9Image2D();
		BOOL Translate(DX9& dx9);
		BOOL Load(DX9& dx9, const CHAR* pzFile);
		BOOL Load(DX9& dx9, const BYTE* bits, LONG size);
		void SetRotate(FLOAT angle);
		void SetScale(const D3DXVECTOR2& scale);
		void SetTranslate(const D3DXVECTOR2& pos);
		void SetFlipH(BOOL bFlag);
		void SetFlipV(BOOL bFlag);
	public:
		BOOL Create(DX9& dx9, INT cx, INT cy, const BYTE* bits) OVERRIDE;
		BOOL Allocate(DX9& dx9) OVERRIDE;
		BOOL Process(DX9& dx9) OVERRIDE;
		void Deallocate(DX9& dx9) OVERRIDE;
	private:
		BOOL Initialize(DX9& dx9);
	protected:
		BOOL								m_bFlipH;
		BOOL								m_bFlipV;
		FLOAT								m_angle;
		D3DXVECTOR2							m_pos;
		D3DXVECTOR2							m_scale;
		TinySize							m_size;
		TinyComPtr<IDirect3DVertexBuffer9>	m_vertexs;
	};
}

