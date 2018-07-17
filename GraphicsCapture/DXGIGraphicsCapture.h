#pragma once
#include "DXCommon.h"
#include "DX10GraphicsCapture.h"
#include "DX101GraphicsCapture.h"
#include "DX11GraphicsCapture.h"

namespace GraphicsCapture
{
	class DXGIGraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DXGIGraphicsCapture)
	public:
		DXGIGraphicsCapture(DX10GraphicsCapture& dx10, DX101GraphicsCapture& dx101, DX11GraphicsCapture& dx11);
		~DXGIGraphicsCapture();
		BOOL Initialize(HWND hWND);
		void Release();
	public:
		TinyDetour	m_dxPresent;
		TinyDetour	m_dxResizeBuffers;
	public:
		BOOL					m_bDX10;
		BOOL					m_bDX101;
		BOOL					m_bDX11;
		IDXGISwapChain*			m_currentSwap;
		DX10GraphicsCapture&	m_dx10;
		DX101GraphicsCapture&	m_dx101;
		DX11GraphicsCapture&	m_dx11;
	};

	SELECTANY extern DXGIGraphicsCapture g_dxgi(g_dx10, g_dx101, g_dx11);
}

