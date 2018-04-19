#pragma once
#include "DX9.h"
#include <Richedit.h>
#include <d3dx9core.h>

namespace DXFramework
{
	/// <summary>
	/// DX9Ïß
	/// </summary>
	class DX9Line2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX9Line2D)
	public:
		DX9Line2D();
		virtual ~DX9Line2D();
		virtual BOOL Create(DX9& dx9);
		virtual void Destory();
	public:
		BOOL		IsEmpty() const;
		BOOL		DrawLine(DX9& dx9, TinyPoint list[4], D3DCOLOR color);
	private:
		TinyComPtr<ID3DXLine>	m_d3dLINE;
	};
}



