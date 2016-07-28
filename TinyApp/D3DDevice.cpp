#include "stdafx.h"
#include "D3DDevice.h"
#include "Vector2D.h"

namespace D3D
{
	CD3DDevice::CD3DDevice()
	{

	}
	CD3DDevice::~CD3DDevice()
	{

	}
	BOOL CD3DDevice::Initialize(HWND hWND, INT cx, INT cy)
	{
		TinyComPtr<IDXGIFactory1> gif1;
		HRESULT hRes = S_OK;
		TinyComPtr<IDXGIAdapter1> adapter;
		if (SUCCEEDED(hRes = CreateDXGIFactory1(IID_IDXGIFactory1, (void**)&gif1)))
		{
			INT index = 0;
			IDXGIAdapter1* gia = NULL;
			while (gif1->EnumAdapters1(index++, &adapter) == S_OK)
				break;
		}
		if (!adapter)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC dscd;
		ZeroMemory(&dscd, sizeof(dscd));
		dscd.BufferCount = 2;
		dscd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		dscd.BufferDesc.Width = cx;
		dscd.BufferDesc.Height = cy;
		dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dscd.OutputWindow = hWND;
		dscd.SampleDesc.Count = 1;
		dscd.Windowed = TRUE;
		dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		hRes = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &dscd, &m_swap, &m_d3d);
		if (FAILED(hRes))
			hRes = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &dscd, &m_swap, &m_d3d);
		if (FAILED(hRes))
			return FALSE;
		//创建渲染目标视图
		TinyComPtr<ID3D10Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, IID_ID3D10Texture2D, (void**)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		//设置光栅器状态
		D3D10_RASTERIZER_DESC drd;
		ZeroMemory(&drd, sizeof(drd));
		drd.FillMode = D3D10_FILL_SOLID;
		drd.CullMode = D3D10_CULL_NONE;
		drd.FrontCounterClockwise = FALSE;
		drd.DepthClipEnable = TRUE;
		hRes = m_d3d->CreateRasterizerState(&drd, &m_rasterizerState);
		if (FAILED(hRes))
			return FALSE;
		m_d3d->RSSetState(m_rasterizerState);
		//设置深度模版状态
		D3D10_DEPTH_STENCIL_DESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.DepthEnable = FALSE;
		hRes = m_d3d->CreateDepthStencilState(&ddsd, &m_depthState);
		if (FAILED(hRes))
			return FALSE;
		m_d3d->OMSetDepthStencilState(m_depthState, 0);
		//设置混合状态
		D3D10_BLEND_DESC dbd;
		ZeroMemory(&dbd, sizeof(dbd));
		for (int i = 0; i < 8; i++)
		{
			dbd.BlendEnable[i] = TRUE;
			dbd.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
		}
		dbd.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		dbd.BlendOp = D3D10_BLEND_OP_ADD;
		dbd.SrcBlendAlpha = D3D10_BLEND_ONE;
		dbd.DestBlendAlpha = D3D10_BLEND_ZERO;
		dbd.SrcBlend = D3D10_BLEND_ONE;
		dbd.DestBlend = D3D10_BLEND_ZERO;
		hRes = m_d3d->CreateBlendState(&dbd, &m_blendState);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void CD3DDevice::SetViewport(FLOAT x, FLOAT y, FLOAT cx, FLOAT cy)
	{
		ASSERT(m_d3d);
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = INT(x);
		vp.TopLeftY = INT(y);
		vp.Width = UINT(cx);
		vp.Height = UINT(cy);
		m_d3d->RSSetViewports(1, &vp);
	}
	void CD3DDevice::SetScissorRect(const D3D10_RECT *pRect)
	{
		ASSERT(m_d3d);
		if (pRect)
		{
			m_d3d->RSSetState(m_scissorState);
			m_d3d->RSSetScissorRects(1, pRect);
		}
		else
		{
			m_d3d->RSSetState(m_rasterizerState);
			m_d3d->RSSetScissorRects(0, NULL);
		}
	}
	BOOL CD3DDevice::ResizeView()
	{
		LPVOID nullVal = NULL;
		m_d3d->OMSetRenderTargets(1, (ID3D10RenderTargetView**)&nullVal, NULL);
		m_renderView.Release();
		m_swap->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		TinyComPtr<ID3D10Texture2D> backBuffer;
		if (FAILED(m_swap->GetBuffer(0, IID_ID3D10Texture2D, (void**)&backBuffer)))
		{
			return FALSE;
		}
		if (FAILED(m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView)))
		{
			return FALSE;
		}
		return TRUE;
	}
	ID3D10Device1*	CD3DDevice::GetD3D() const
	{
		return m_d3d;
	}
	void CD3DDevice::CopyTexture(CD3D10Texture *texDest, CD3D10Texture *texSrc)
	{
		CD3D10Texture *d3d10Dest = static_cast<CD3D10Texture*>(texDest);
		CD3D10Texture *d3d10Src = static_cast<CD3D10Texture*>(texSrc);
		m_d3d->CopyResource(d3d10Dest->m_d3d10Texture2D, d3d10Src->m_d3d10Texture2D);
	}
	void CD3DDevice::DrawSprite(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2)
	{
		ASSERT(texture);
		DrawSpriteEx(texture, color, x, y, x2, y2, 0.0f, 0.0f, 1.0f, 1.0f);
	}
	void CD3DDevice::DrawSpriteEx(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2, float u, float v, float u2, float v2)
	{

	}
}

