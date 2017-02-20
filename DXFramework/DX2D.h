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
	public:
		DX2D();
		~DX2D();
		BOOL Initialize(HWND hWND, INT x, INT y, INT cx, INT cy);
	private:
		HWND							m_hWND;
		TinyPoint						m_pos;
		TinySize						m_size;
		TinyComPtr<ID2D1DeviceContext>	m_target;
		TinyComPtr<IDXGISwapChain1>		m_swap;
		TinyComPtr<ID2D1Factory1>		m_factory;
		TinyComPtr<ID3D11Device>		m_d3d;
	};
}



