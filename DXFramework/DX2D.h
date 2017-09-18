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
		BOOL BeginDraw();
		BOOL EndDraw();
		BOOL Resize(INT cx, INT cy);
		ID2D1HwndRenderTarget* GetCanvas() const;
		HWND GetHWND() const;
	private:
		HWND								m_hWND;
		TinyComPtr<ID2D1Factory>			m_factory;
		TinyComPtr<ID2D1HwndRenderTarget>	m_renderTarget;
	};
}



