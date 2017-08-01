#include "stdafx.h"
#include "DX11.h"
#include "DX11RenderView.h"

namespace DXFramework
{
	DX11::DX11()
		:m_hWND(NULL),
		m_render2D(NULL)
	{

	}
	DX11::~DX11()
	{

	}
	BOOL DX11::Initialize(HWND hWND, INT cx, INT cy)
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
		hRes = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, dwFlag, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &swapDesc, &m_swap, &m_d3d, &level, &m_immediateContext);
		if (hRes != S_OK)
			return FALSE;
		m_background2D.Reset(new DX11RenderView(*this));
		if (!m_background2D->Create())
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), m_background2D->GetSize());
		this->SetMatrixs(m_background2D->GetSize());
		m_render2D = m_background2D;
		D3D11_DEPTH_STENCIL_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDesc.StencilEnable = TRUE;
		depthDesc.StencilReadMask = 0xFF;
		depthDesc.StencilWriteMask = 0xFF;
		depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
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
		m_immediateContext->OMSetDepthStencilState(m_enableDepthState, 1);
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
		for (INT i = 0;i < 8;i++)
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
		for (INT i = 0;i < 8;i++)
		{
			blenddesc.RenderTarget[i].BlendEnable = FALSE;
			blenddesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blenddesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			blenddesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		hRes = m_d3d->CreateBlendState(&blenddesc, &m_disableBlendState);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11::ResizeView(INT cx, INT cy)
	{
		if (!IsValid())
			return FALSE;
		if (!m_background2D->Resize())
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), m_background2D->GetSize());
		this->SetMatrixs(m_background2D->GetSize());
		return TRUE;
	}
	void DX11::AllowBlend(BOOL bAllow, FLOAT(&blendFactor)[4])
	{
		m_immediateContext->OMSetBlendState(bAllow ? m_enableBlendState : m_disableBlendState, blendFactor, 0xFFFFFFFF);
	}
	void DX11::AllowDepth(BOOL bAllow)
	{
		m_immediateContext->OMSetDepthStencilState(bAllow ? m_enableDepthState : m_disableDepthState, 1);
	}
	void DX11::SetRenderTexture2D(DX11RenderView* render2D)
	{
		if (render2D == NULL)
		{
			m_render2D = m_background2D;
		}
		else
		{
			if (m_render2D != render2D)
			{
				m_render2D = render2D;
			}
		}
	}
	void DX11::Present()
	{
		if (IsValid())
		{
			m_swap->Present(0, 0);
		}
	}
	BOOL DX11::IsValid() const
	{
		if (m_immediateContext != NULL &&
			m_swap != NULL &&
			m_d3d != NULL)
			return TRUE;
		return FALSE;
	}
	TinySize DX11::GetSize() const
	{
		return m_background2D->GetSize();
	}
	ID3D11Device* DX11::GetD3D() const
	{
		return m_d3d;
	}
	ID3D11DeviceContext* DX11::GetImmediateContext() const
	{
		return m_immediateContext;
	}
	IDXGISwapChain*	DX11::GetSwap() const
	{
		return m_swap;
	}
	DX11RenderView* DX11::GetRender2D() const
	{
		return m_render2D;
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
		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 1000.0F, 0.1F);
		m_matrixs[1] = XMMatrixIdentity();
		m_matrixs[2] = XMMatrixOrthographicLH((FLOAT)size.cx, (FLOAT)size.cy, 1000.0F, 0.1F);
	}
	BOOL DX11::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (!m_immediateContext)
			return FALSE;
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = static_cast<FLOAT>(pos.x);
		viewport.TopLeftY = static_cast<FLOAT>(pos.y);
		viewport.Width = static_cast<FLOAT>(size.cx);
		viewport.Height = static_cast<FLOAT>(size.cy);
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		m_immediateContext->RSSetViewports(1, &viewport);
		return TRUE;
	}
}
