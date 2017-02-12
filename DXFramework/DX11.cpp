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
	BOOL DX11::Initialize(HWND hWND, INT x, INT y, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_pos.x = x;
		m_pos.y = y;
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
		DWORD dwFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_9_3;
		hRes = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, dwFlag, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &swapDesc, &m_swap, &m_d3d, &level, &m_immediateContext);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = cx;
		depthBufferDesc.Height = cy;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hRes = m_d3d->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(hRes))
			return FALSE;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = TRUE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(hRes))
			return FALSE;
		m_immediateContext->OMSetDepthStencilState(m_depthStencilState, 1);
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hRes = m_d3d->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hRes))
			return FALSE;
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		hRes = m_d3d->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
		if (FAILED(hRes))
			return FALSE;
		m_immediateContext->RSSetState(m_rasterizerState);
		D3D11_VIEWPORT viewport = { 0 };
		viewport.Width = static_cast<FLOAT>(cx);
		viewport.Height = static_cast<FLOAT>(cy);
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		viewport.TopLeftX = 0.0F;
		viewport.TopLeftY = 0.0F;
		m_immediateContext->RSSetViewports(1, &viewport);
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)cx / (FLOAT)cy;
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect, 1000.0F, 0.1F);
		m_worldMatrix = XMMatrixIdentity();
		m_orthoMatrix = XMMatrixOrthographicLH((FLOAT)cx, (FLOAT)cy, 1000.0F, 0.1F);
		D3D11_DEPTH_STENCIL_DESC disableDepthStencilDesc;
		ZeroMemory(&disableDepthStencilDesc, sizeof(disableDepthStencilDesc));
		disableDepthStencilDesc.DepthEnable = FALSE;
		disableDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		disableDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		disableDepthStencilDesc.StencilEnable = TRUE;
		disableDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		disableDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		disableDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		disableDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		disableDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&disableDepthStencilDesc, &m_disableDepthState);
		if (FAILED(hRes))
			return FALSE;
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
		if (FAILED(hRes))
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
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		m_depthStencilBuffer.Release();
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = m_size.cx;
		depthBufferDesc.Height = m_size.cy;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hRes = m_d3d->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(hRes))
			return FALSE;
		m_depthStencilView.Release();
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hRes = m_d3d->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hRes))
			return FALSE;
		m_immediateContext->OMSetRenderTargets(1, &m_renderView, m_depthStencilView);
		//更新视口
		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<FLOAT>(m_size.cx);
		viewport.Height = static_cast<FLOAT>(m_size.cy);
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		viewport.TopLeftX = 0.0F;
		viewport.TopLeftY = 0.0F;
		m_immediateContext->RSSetViewports(1, &viewport);
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)m_size.cx / (FLOAT)m_size.cy;
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect, 1000.0F, 0.1F);
		m_worldMatrix = XMMatrixIdentity();
		m_orthoMatrix = XMMatrixOrthographicLH((FLOAT)cx, (FLOAT)cy, 1000.0F, 0.1F);
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
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX11::BeginScene()
	{
		if (m_immediateContext)
		{
			if (m_renderView)
			{
				FLOAT color[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
				m_immediateContext->ClearRenderTargetView(m_renderView, color);
			}
			if (m_depthStencilView)
			{
				m_immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0F, 0);
			}
		}
	}
	void DX11::EndScene()
	{
		if (m_immediateContext && m_swap)
		{
			m_swap->Present(0, 0);
			m_immediateContext->Flush();
		}
	}
	void DX11::AllowDepth(BOOL allow)
	{
		if (m_immediateContext)
		{
			m_immediateContext->OMSetDepthStencilState(allow ? m_depthStencilState : m_disableDepthState, 1);
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
		return m_depthStencilView;
	}
	HWND DX11::GetHWND() const
	{
		return m_hWND;
	}
	TinySize DX11::GetSize() const
	{
		return m_size;
	}
	XMMATRIX DX11::GetProjectionMatrix()
	{
		return m_projectionMatrix;
	}
	XMMATRIX DX11::GetWorldMatrix()
	{
		return m_worldMatrix;
	}
	XMMATRIX DX11::GetOrthoMatrix()
	{
		return m_orthoMatrix;
	}
}
