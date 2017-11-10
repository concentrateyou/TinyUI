#pragma once
#include "DXFramework.h"
#include "DX9.h"
#include "DX9Image2D.h"

namespace DXFramework
{
	/// <summary>
	/// DX9����
	/// </summary>
	class DX9Graphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX9Graphics2D)
	public:
		DX9Graphics2D();
		virtual ~DX9Graphics2D();
		DX9&	GetDX9();
		BOOL	Reset();
		BOOL	Present();
		BOOL	Initialize(HWND hWND, const TinySize& size);
		BOOL	DrawImage(DX9Image2D* ps, FLOAT ratioX = 1.0F, FLOAT ratioY = 1.0F);
		BOOL	DrawString(LPCSTR pzText, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color);
		void	SetRenderView(DX9RenderView* render2D);
		void	Enter();
		void	Leave();
	public:
		BOOL			IsActive() const;
		DX9RenderView*	GetRenderView() const;
	private:
		DX9		m_dx9;
	};
}

