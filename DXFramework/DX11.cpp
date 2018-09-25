#include "stdafx.h"
#include "DX11.h"
#include "DX11Graphics2D.h"
#include "Common/TinyLogging.h"

namespace DXFramework
{
	DX11::DX11()
		:m_hWND(NULL)
	{

	}
	DX11::~DX11()
	{

	}
	BOOL DX11::EnumAdapters(vector<IDXGIAdapter*>& adapters)
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
	BOOL DX11::Initialize(HWND hWND, INT cx, INT cy, IDXGIAdapter* pAdapter)
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
		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3
		};
		DWORD dwFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		dwFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_9_3;
		hRes = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_HARDWARE, NULL, dwFlag, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &swapDesc, &m_swap, &m_d3d, &level, &m_immediateContext);
		if (hRes != S_OK)
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), { cx,cy });
		this->SetMatrixs({ cx,cy });
		D3D11_DEPTH_STENCIL_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDesc.StencilEnable = TRUE;
		depthDesc.StencilReadMask = 0xFF;
		depthDesc.StencilWriteMask = 0xFF;
		depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&depthDesc, &m_enableDepthState);
		if (hRes != S_OK)
			return FALSE;
		depthDesc.DepthEnable = FALSE;
		depthDesc.StencilEnable = FALSE;
		hRes = m_d3d->CreateDepthStencilState(&depthDesc, &m_disableDepthState);
		if (hRes != S_OK)
			return FALSE;
		//�ر����
		m_immediateContext->OMSetDepthStencilState(m_disableDepthState, 1);
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FrontCounterClockwise = TRUE;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		hRes = m_d3d->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
		if (hRes != S_OK)
			return FALSE;
		m_immediateContext->RSSetState(m_rasterizerState);
		D3D11_BLEND_DESC blenddesc;
		ZeroMemory(&blenddesc, sizeof(blenddesc));
		for (INT i = 0; i < 8; i++)
		{
			blenddesc.RenderTarget[i].BlendEnable = TRUE;
			blenddesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blenddesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			blenddesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		hRes = m_d3d->CreateBlendState(&blenddesc, &m_enableBlendState);
		if (hRes != S_OK)
			return FALSE;
		for (INT i = 0; i < 8; i++)
		{
			blenddesc.RenderTarget[i].BlendEnable = FALSE;
			blenddesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blenddesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			blenddesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		hRes = m_d3d->CreateBlendState(&blenddesc, &m_disableBlendState);
		if (hRes != S_OK)
			return FALSE;
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_immediateContext->OMSetBlendState(m_enableBlendState, blendFactor, 0xFFFFFFFF);
		D3D11_FEATURE_DATA_THREADING option = { 0 };
		hRes = m_d3d->CheckFeatureSupport(D3D11_FEATURE_THREADING, reinterpret_cast<void*>(&option), sizeof(option));
		if (hRes != S_OK)
			return FALSE;
		m_size.cx = cx;
		m_size.cy = cy;
		return TRUE;
	}
	void DX11::Uninitialize()
	{
		m_disableDepthState.Release();
		m_enableDepthState.Release();
		m_disableBlendState.Release();
		m_enableBlendState.Release();
		m_rasterizerState.Release();
		m_immediateContext.Release();
		m_swap.Release();
		m_d3d.Release();
	}
	BOOL DX11::Resize(INT cx, INT cy)
	{
		if (IsEmpty())
			return FALSE;
		LPVOID ps = NULL;
		m_immediateContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&ps, NULL);
		HRESULT hRes = m_swap->ResizeBuffers(2, cx, cy, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (hRes != S_OK)
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), { cx,cy });
		this->SetMatrixs({ cx,cy });
		return TRUE;
	}
	BOOL DX11::AllowBlend(BOOL bAllow, FLOAT blendFactor[4])
	{
		if (IsEmpty())
			return FALSE;
		m_immediateContext->OMSetBlendState(bAllow ? m_enableBlendState : m_disableBlendState, blendFactor, 0xFFFFFFFF);
		return TRUE;
	}
	BOOL DX11::AllowDepth(BOOL bAllow)
	{
		if (IsEmpty())
			return FALSE;
		m_immediateContext->OMSetDepthStencilState(bAllow ? m_enableDepthState : m_disableDepthState, 1);
		return TRUE;
	}
	BOOL DX11::Present()
	{
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_swap->Present(0, 0);
		return SUCCEEDED(hRes);
	}
	BOOL DX11::Flush()
	{
		if (IsEmpty())
			return FALSE;
		m_immediateContext->Flush();
		return TRUE;
	}
	BOOL DX11::IsEmpty() const
	{
		if (m_immediateContext != NULL &&
			m_swap != NULL &&
			m_d3d != NULL)
			return FALSE;
		return TRUE;
	}
	TinySize DX11::GetSize() const
	{
		return m_size;
	}
	ID3D11Device* DX11::GetD3D() const
	{
		return m_d3d;
	}
	ID3D11DeviceContext* DX11::GetImmediateContext() const
	{
		return m_immediateContext;
	}
	IDXGISwapChain*	DX11::GetSwapChain() const
	{
		return m_swap;
	}
	HWND DX11::GetHWND() const
	{
		return m_hWND;
	}
	XMMATRIX* DX11::GetMatrixs()
	{
		return m_matrixs;
	}
	void DX11::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 0.1F, 1000.0F);
		m_matrixs[1] = XMMatrixIdentity();//WORLD
		m_matrixs[2] = XMMatrixOrthographicLH((FLOAT)size.cx, (FLOAT)size.cy, 0.1F, 1000.0F);//PROJECTION
	}
	BOOL DX11::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (IsEmpty())
			return FALSE;
		D3D11_VIEWPORT vp;
		vp.TopLeftX = static_cast<FLOAT>(pos.x);
		vp.TopLeftY = static_cast<FLOAT>(pos.y);
		vp.Width = static_cast<FLOAT>(size.cx);
		vp.Height = static_cast<FLOAT>(size.cy);
		vp.MinDepth = 0.0F;
		vp.MaxDepth = 1.0F;
		m_immediateContext->RSSetViewports(1, &vp);
		return TRUE;
	}
	BOOL DX11::ReportLiveObjects()
	{
		TinyComPtr<ID3D11Debug> m_debug;
		HRESULT hRes = m_d3d->QueryInterface(&m_debug);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
}
