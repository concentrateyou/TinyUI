#pragma once
#include "DXFramework.h"
#include "DX9.h"
#include "DX9Image2D.h"
#include "DX9Font2D.h"
#include "DX9Line2D.h"

namespace DXFramework
{
	/// <summary>
	/// DX9»­²¼
	/// </summary>
	class DX9Graphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX9Graphics2D)
	public:
		DX9Graphics2D();
		virtual ~DX9Graphics2D();
		DX9&			GetDX9();
		BOOL			Reset();
		BOOL			Present();
		BOOL			Initialize(HWND hWND, const TinySize& size);
	public:
		BOOL			DrawImage(DX9Image2D* ps);
		BOOL			DrawLine(DX9Line2D* ps, D3DXVECTOR2 list[4], D3DCOLOR color);
		BOOL			DrawString(DX9Font2D* ps, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color);
	public:
		BOOL			IsActive() const;
		void			SetRenderView(DX9RenderView* render2D);
		DX9RenderView*	GetRenderView() const;
	private:
		DX9				m_dx9;
	};
}

