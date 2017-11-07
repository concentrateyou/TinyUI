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
		virtual ~DX2D();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		BOOL BeginDraw();
		BOOL EndDraw(HRESULT& hRes);
		BOOL Resize(INT cx, INT cy);
		ID2D1HwndRenderTarget* GetCanvas() const;
		HWND GetHWND() const;
	private:
		BOOL Create(HWND hWND, INT cx, INT cy);
	private:
		TinySize							m_size;
		HWND								m_hWND;
		TinyComPtr<ID2D1Factory>			m_factory;
		TinyComPtr<ID2D1HwndRenderTarget>	m_hwndRenderTarget;
	};
}



