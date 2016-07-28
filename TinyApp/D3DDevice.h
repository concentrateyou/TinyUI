#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <DXGI.h>
#include "Common/TinyCommon.h"
#include "Common/TinyCore.h"
#include "IO/TinySharedMemory.h"
#include "D3D10Texture.h"
using namespace TinyUI;

enum GSBlendType { GS_BLEND_ZERO, GS_BLEND_ONE, GS_BLEND_SRCCOLOR, GS_BLEND_INVSRCCOLOR, GS_BLEND_SRCALPHA, GS_BLEND_INVSRCALPHA, GS_BLEND_DSTCOLOR, GS_BLEND_INVDSTCOLOR, GS_BLEND_DSTALPHA, GS_BLEND_INVDSTALPHA, GS_BLEND_FACTOR, GS_BLEND_INVFACTOR };

namespace D3D
{
	class CD3D10Texture;
	class CD3DDevice
	{
	public:
		CD3DDevice();
		~CD3DDevice();
		BOOL Initialize(HWND hWND, INT cx, INT cy);//D3D标准化的初始化过程
		void SetViewport(FLOAT x, FLOAT y, FLOAT cx, FLOAT cy);
		void SetScissorRect(const D3D10_RECT *pRects);
		BOOL ResizeView();
		ID3D10Device1*	GetD3D() const;
		void CopyTexture(CD3D10Texture *texDest, CD3D10Texture *texSrc);
		void DrawSprite(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2);
		void DrawSpriteEx(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2, float u, float v, float u2, float v2);
	private:
		IO::TinySharedMemory						m_sharedCapture;
		DXGI_SWAP_CHAIN_DESC						m_dsc;
		TinyUI::TinyComPtr<IDXGISwapChain>			m_swap;
		TinyUI::TinyComPtr<ID3D10Device1>			m_d3d;
		TinyUI::TinyComPtr<ID3D10DepthStencilState> m_depthState;
		TinyUI::TinyComPtr<ID3D10RasterizerState>	m_rasterizerState;
		TinyUI::TinyComPtr<ID3D10RasterizerState>   m_scissorState;
		TinyUI::TinyComPtr<ID3D10RenderTargetView>  m_renderView;
		TinyUI::TinyComPtr<ID3D10BlendState>        m_blendState;
	};
}

