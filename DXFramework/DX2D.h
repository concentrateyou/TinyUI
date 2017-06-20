#pragma once
#include "DXFramework.h"
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>

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
		BOOL BeginDraw(ID2D1Bitmap1* bitmap = NULL);
		BOOL EndDraw();
		BOOL Resize();
		ID2D1DeviceContext* GetContext() const;
		HWND GetHWND() const;
	private:
		HWND							m_hWND;
		TinyComPtr<ID2D1Factory1>		m_factory;
		TinyComPtr<IDXGISwapChain1>		m_swap;
		TinyComPtr<ID3D11Device>		m_d3d;
		TinyComPtr<ID2D1DeviceContext>	m_context;
		TinyComPtr<ID2D1Bitmap1>		m_bitmap;
	};
}



