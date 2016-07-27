#pragma once
#include <DXGI.h>
#include <d3d.h>
#include <d3d10_1.h>
#include "Common/TinyCommon.h"
#include "Common/TinyCore.h"
#include "IO/TinySharedMemory.h"
using namespace TinyUI;

namespace D3D
{
	class CD3DSystem
	{
	public:
		CD3DSystem();
		~CD3DSystem();
		BOOL Initialize(HWND hWND, INT cx, INT cy);//D3D标准化的初始化过程
		void SetViewport(FLOAT x, FLOAT y, FLOAT cx, FLOAT cy);
		void SetScissorRect(const D3D10_RECT *pRects);
		ID3D10Device1*	GetD3D() const;
	private:
		IO::TinySharedMemory				m_sharedCapture;
		DXGI_SWAP_CHAIN_DESC				m_dsc;
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D10Device1>			m_d3d;
		TinyComPtr<ID3D10DepthStencilState> m_depthState;
		TinyComPtr<ID3D10RasterizerState>	m_rasterizerState;
		TinyComPtr<ID3D10RasterizerState>   m_scissorState;
		TinyComPtr<ID3D10RenderTargetView>  m_renderView;
		TinyComPtr<ID3D10BlendState>        m_blendState;
	};
}

