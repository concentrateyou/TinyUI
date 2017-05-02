#include "stdafx.h"
#include "DX11.h"
#include "DX11RenderTexture2D.h"

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
		m_back2D.Reset(new DX11RenderTexture2D(*this));
		if (!m_back2D->Create())
			return FALSE;
		D3D11_DEPTH_STENCIL_DESC enableDepthDesc;
		ZeroMemory(&enableDepthDesc, sizeof(enableDepthDesc));
		enableDepthDesc.DepthEnable = TRUE;
		enableDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		enableDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		enableDepthDesc.StencilEnable = TRUE;
		enableDepthDesc.StencilReadMask = 0xFF;
		enableDepthDesc.StencilWriteMask = 0xFF;
		enableDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		enableDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		enableDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		enableDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		enableDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		enableDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		enableDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		enableDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&enableDepthDesc, &m_depthStencilState);
		if (hRes != S_OK)
			return FALSE;
		m_immediateContext->OMSetDepthStencilState(m_depthStencilState, 1);
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		hRes = m_d3d->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
		if (hRes != S_OK)
			return FALSE;
		m_immediateContext->RSSetState(m_rasterizerState);
		return TRUE;
	}
	BOOL DX11::ResizeView(INT cx, INT cy)
	{
		if (!IsValid())
			return FALSE;
		if (!m_back2D->Resize())
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), m_back2D->GetSize());
		this->SetMatrixs(m_back2D->GetSize());
		return TRUE;
	}
	void DX11::SetRenderTexture2D(DX11RenderTexture2D* render2D)
	{
		if (render2D == NULL)
		{
			m_render2D = m_back2D;
		}
		else
		{
			if (m_render2D != m_back2D)
			{
				m_render2D = m_back2D;
			}
		}
		this->SetViewport(TinyPoint(0, 0), m_render2D->GetSize());
		this->SetMatrixs(m_render2D->GetSize());
	}
	void DX11::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 1000.0F, 0.1F);
		m_matrixs[1] = XMMatrixIdentity();
		m_matrixs[2] = XMMatrixOrthographicLH((FLOAT)size.cx, (FLOAT)size.cy, 1000.0F, 0.1F);
	}
	void DX11::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (IsValid())
		{
			D3D11_VIEWPORT viewport;
			viewport.Width = static_cast<FLOAT>(size.cx);
			viewport.Height = static_cast<FLOAT>(size.cy);
			viewport.MinDepth = 0.0F;
			viewport.MaxDepth = 1.0F;
			viewport.TopLeftX = static_cast<FLOAT>(pos.x);
			viewport.TopLeftY = static_cast<FLOAT>(pos.y);
			m_immediateContext->RSSetViewports(1, &viewport);
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
		if (m_immediateContext && m_swap && m_d3d)
			return TRUE;
		return FALSE;
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
	DX11RenderTexture2D* DX11::GetRender2D() const
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
}
