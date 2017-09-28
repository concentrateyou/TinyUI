#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	/// <summary>
	/// https://msdn.microsoft.com/magazine/dn198239
	/// </summary>
	class DX2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX2D)
	public:
		DX2D();
		~DX2D();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		BOOL Initialize(HDC hDC, INT cx, INT cy);
		BOOL BeginDraw();
		BOOL EndDraw();
		BOOL Resize(INT cx, INT cy);
		ID2D1RenderTarget* GetCanvas() const;
		HWND GetHWND() const;
		HDC  GetDC() const;
	private:
		HWND								m_hWND;
		HDC									m_hDC;
		TinyComPtr<ID2D1Factory>			m_factory;
		TinyComPtr<ID2D1HwndRenderTarget>	m_hwndRenderTarget;
		TinyComPtr<ID2D1DCRenderTarget>		m_dcRenderTarget;
	};
}



