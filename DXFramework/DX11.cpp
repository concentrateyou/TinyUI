#include "stdafx.h"
#include "DX11.h"

namespace DXFramework
{
	DX11::DX11()
		:m_hWND(NULL)
	{

	}
	DX11::~DX11()
	{

	}
	BOOL DX11::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_size.cx = cx;
		m_size.cy = cy;
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
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = cx;
		depthDesc.Height = cy;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_d3d->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
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
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_d3d->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
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
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_size.cx;
		desc.Height = m_size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		desc.BindFlags = 0;
		hRes = m_d3d->CreateTexture2D(&desc, NULL, &m_renderTexture);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX11::ResizeView(INT cx, INT cy)
	{
		if (!m_immediateContext || !m_swap || !m_d3d)
			return FALSE;
		m_size.cx = cx;
		m_size.cy = cy;
		LPVOID val = NULL;
		m_immediateContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&val, NULL);
		m_renderView.Release();
		HRESULT hRes = m_swap->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		m_depth2D.Release();
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = m_size.cx;
		depthDesc.Height = m_size.cy;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_d3d->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		m_depthView.Release();
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_d3d->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		m_renderTexture.Release();
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_size.cx;
		desc.Height = m_size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		desc.BindFlags = 0;
		hRes = m_d3d->CreateTexture2D(&desc, NULL, &m_renderTexture);
		if (hRes != S_OK)
			return FALSE;
		//////////////////////////////////////////////////////////////////////////
		this->SetViewport(TinyPoint(0, 0), m_size);
		this->SetMatrixs(m_size);
		return TRUE;
	}
	void DX11::BeginDraw()
	{
		if (m_immediateContext != NULL &&
			m_renderView != NULL &&
			m_depthView != NULL)
		{
			m_immediateContext->OMSetRenderTargets(1, &m_renderView, m_depthView);
			FLOAT color[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
			m_immediateContext->ClearRenderTargetView(m_renderView, color);
			m_immediateContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH, 1.0F, 0);
		}
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
		viewport.Width = static_cast<FLOAT>(size.cx);
		viewport.Height = static_cast<FLOAT>(size.cy);
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		viewport.TopLeftX = static_cast<FLOAT>(pos.x);
		viewport.TopLeftY = static_cast<FLOAT>(pos.y);
		m_immediateContext->RSSetViewports(1, &viewport);
		return TRUE;
	}
	void DX11::EndDraw()
	{
		if (m_swap != NULL)
		{
			m_swap->Present(0, 0);
		}
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
	ID3D11Texture2D* DX11::GetTexture2D() const
	{
		return m_renderTexture;
	}
	ID3D11DepthStencilView* DX11::GetDSView() const
	{
		return m_depthView;
	}
	ID3D11RenderTargetView*	DX11::GetRTView() const
	{
		return m_renderView;
	}
	HWND DX11::GetHWND() const
	{
		return m_hWND;
	}
	TinySize DX11::GetSize() const
	{
		return m_size;
	}
	XMMATRIX* DX11::GetMatrixs()
	{
		return m_matrixs;
	}
}
