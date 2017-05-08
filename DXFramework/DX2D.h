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
		BOOL BeginDraw();
		BOOL EndDraw();
		BOOL Resize();
		ID2D1DeviceContext* GetContext() const;
	private:
		HWND							m_hWND;
		TinySize						m_size;
		TinyComPtr<ID2D1DeviceContext>	m_context;
		TinyComPtr<IDXGISwapChain1>		m_swap;
		TinyComPtr<ID2D1Factory1>		m_factory;
		TinyComPtr<ID3D11Device>		m_d3d;
	};
}



