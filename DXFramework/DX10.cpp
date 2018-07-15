#include "stdafx.h"
#include "DX10.h"
#include "DX10Graphics2D.h"
#include "Common/TinyLogging.h"

namespace DXFramework
{
	DX10::DX10()
		:m_hWND(NULL)
	{

	}
	DX10::~DX10()
	{

	}
	BOOL DX10::EnumAdapters(vector<IDXGIAdapter*>& adapters)
	{
		TinyComPtr<IDXGIFactory> factory;
		HRESULT hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
		if (hRes != S_OK)
			return FALSE;
		IDXGIAdapter* adapter;
		UINT index = 0;
		while (factory->EnumAdapters(index, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			adapters.push_back(adapter);
			++index;
		}
		return TRUE;
	}
	BOOL DX10::Initialize(HWND hWND, INT cx, INT cy, IDXGIAdapter* pAdapter)
	{
		m_hWND = hWND;
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 2;
		swapDesc.BufferDesc.Width = cx;
		swapDesc.BufferDesc.Height = cy;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = hWND;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.Windowed = TRUE;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
		HRESULT hRes = S_OK;
		DWORD dwFlag = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
		dwFlag |= D3D10_CREATE_DEVICE_DEBUG;
#endif
		hRes = D3D10CreateDeviceAndSwapChain(pAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, dwFlag, D3D10_SDK_VERSION, &swapDesc, &m_swap, &m_d3d);
		if (hRes != S_OK)
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), { cx,cy });
		this->SetMatrixs({ cx,cy });
		D3D10_DEPTH_STENCIL_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthDesc.StencilEnable = TRUE;
		depthDesc.StencilReadMask = 0xFF;
		depthDesc.StencilWriteMask = 0xFF;
		depthDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&depthDesc, &m_enableDepthState);
		if (hRes != S_OK)
			return FALSE;
		depthDesc.DepthEnable = FALSE;
		depthDesc.StencilEnable = FALSE;
		hRes = m_d3d->CreateDepthStencilState(&depthDesc, &m_disableDepthState);
		if (hRes != S_OK)
			return FALSE;
		m_d3d->OMSetDepthStencilState(m_enableDepthState, 1);
		D3D10_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FrontCounterClockwise = TRUE;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.FillMode = D3D10_FILL_SOLID;
		rasterDesc.CullMode = D3D10_CULL_NONE;
		hRes = m_d3d->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
		if (hRes != S_OK)
			return FALSE;
		m_d3d->RSSetState(m_rasterizerState);
		D3D10_BLEND_DESC blenddesc;
		ZeroMemory(&blenddesc, sizeof(blenddesc));
		blenddesc.BlendEnable[0] = TRUE;
		blenddesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
		blenddesc.BlendOp = D3D10_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D10_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D10_BLEND_ONE;
		blenddesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		blenddesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		hRes = m_d3d->CreateBlendState(&blenddesc, &m_enableBlendState);
		if (hRes != S_OK)
			return FALSE;
		blenddesc.BlendEnable[0] = FALSE;
		blenddesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
		blenddesc.BlendOp = D3D10_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
		blenddesc.DestBlendAlpha = D3D10_BLEND_ONE;
		blenddesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		blenddesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		hRes = m_d3d->CreateBlendState(&blenddesc, &m_disableBlendState);
		if (hRes != S_OK)
			return FALSE;
		m_size.cx = cx;
		m_size.cy = cy;
		return TRUE;
	}
	void DX10::Uninitialize()
	{
		m_disableDepthState.Release();
		m_enableDepthState.Release();
		m_disableBlendState.Release();
		m_enableBlendState.Release();
		m_rasterizerState.Release();
		m_swap.Release();
		m_d3d.Release();
	}
	BOOL DX10::Resize(INT cx, INT cy)
	{
		if (IsEmpty())
			return FALSE;
		LPVOID ps = NULL;
		m_d3d->OMSetRenderTargets(1, (ID3D10RenderTargetView**)&ps, NULL);
		HRESULT hRes = m_swap->ResizeBuffers(2, cx, cy, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (hRes != S_OK)
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), { cx,cy });
		this->SetMatrixs({ cx,cy });
		return TRUE;
	}
	BOOL DX10::AllowBlend(BOOL bAllow, FLOAT blendFactor[4])
	{
		if (IsEmpty())
			return FALSE;
		m_d3d->OMSetBlendState(bAllow ? m_enableBlendState : m_disableBlendState, blendFactor, 0xFFFFFFFF);
		return TRUE;
	}
	BOOL DX10::AllowDepth(BOOL bAllow)
	{
		if (IsEmpty())
			return FALSE;
		m_d3d->OMSetDepthStencilState(bAllow ? m_enableDepthState : m_disableDepthState, 1);
		return TRUE;
	}
	BOOL DX10::Present()
	{
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_swap->Present(0, 0);
		return SUCCEEDED(hRes);
	}
	BOOL DX10::Flush()
	{
		if (IsEmpty())
			return FALSE;
		m_d3d->Flush();
		return TRUE;
	}
	BOOL DX10::IsEmpty() const
	{
		if (m_swap != NULL &&
			m_d3d != NULL)
			return FALSE;
		return TRUE;
	}
	TinySize DX10::GetSize() const
	{
		return m_size;
	}
	ID3D10Device* DX10::GetD3D() const
	{
		return m_d3d;
	}
	IDXGISwapChain*	DX10::GetSwapChain() const
	{
		return m_swap;
	}
	HWND DX10::GetHWND() const
	{
		return m_hWND;
	}
	XMMATRIX* DX10::GetMatrixs()
	{
		return m_matrixs;
	}
	void DX10::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 0.1F, 1000.0F);
		m_matrixs[1] = XMMatrixIdentity();//WORLD
		m_matrixs[2] = XMMatrixOrthographicLH((FLOAT)size.cx, (FLOAT)size.cy, 0.1F, 1000.0F);//PROJECTION
	}
	BOOL DX10::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (IsEmpty())
			return FALSE;
		D3D10_VIEWPORT vp;
		vp.TopLeftX = static_cast<FLOAT>(pos.x);
		vp.TopLeftY = static_cast<FLOAT>(pos.y);
		vp.Width = static_cast<FLOAT>(size.cx);
		vp.Height = static_cast<FLOAT>(size.cy);
		vp.MinDepth = 0.0F;
		vp.MaxDepth = 1.0F;
		m_d3d->RSSetViewports(1, &vp);
		return TRUE;
	}
}
