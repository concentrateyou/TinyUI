#include "stdafx.h"
#include "DX10.h"

namespace DXFramework
{
	DX10::DX10()
		:m_hWND(NULL)
	{
		m_size.cx = m_size.cy = 0;
		m_pos.x = m_pos.y = 0;
	}
	DX10::~DX10()
	{
	}
	BOOL DX10::Initialize(HWND hWND, INT x, INT y, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_pos.x = x;
		m_pos.y = y;
		m_size.cx = cx;
		m_size.cy = cy;
		if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&m_factory)))
			return FALSE;
		TinyComPtr<IDXGIAdapter1> adapter;
		if (FAILED(m_factory->EnumAdapters1(0, &adapter)))
			return FALSE;
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 2;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.Width = cx;
		swapDesc.BufferDesc.Height = cy;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;//不固定垂直刷新率
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.OutputWindow = hWND;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.Windowed = TRUE;
		HRESULT hRes = S_OK;
		hRes = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &swapDesc, &m_swap, &m_d3d);
		if (FAILED(hRes))
			return FALSE;
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
		hRes = m_d3d->CreateDepthStencilState(&depthDesc, &m_depthStencilState);
		if (FAILED(hRes))
			return FALSE;
		m_d3d->OMSetDepthStencilState(m_depthStencilState, 0);
		D3D10_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = cx;
		depthBufferDesc.Height = cy;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hRes = m_d3d->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(hRes))
			return FALSE;
		D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hRes = m_d3d->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hRes))
			return FALSE;
		m_d3d->OMSetRenderTargets(1, &m_renderView, m_depthStencilView);
		//禁用背面消隐
		D3D10_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.CullMode = D3D10_CULL_BACK;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0F;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.FillMode = D3D10_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.SlopeScaledDepthBias = 0.0F;
		hRes = m_d3d->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
		if (FAILED(hRes))
			return FALSE;
		m_d3d->RSSetState(m_rasterizerState);
		TinyComPtr<ID3D10Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		D3D10_DEPTH_STENCIL_DESC disableDepthStencilDesc;
		ZeroMemory(&disableDepthStencilDesc, sizeof(disableDepthStencilDesc));
		disableDepthStencilDesc.DepthEnable = FALSE;
		disableDepthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		disableDepthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		disableDepthStencilDesc.StencilEnable = TRUE;
		disableDepthStencilDesc.StencilReadMask = 0xFF;
		disableDepthStencilDesc.StencilWriteMask = 0xFF;
		disableDepthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		disableDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		disableDepthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		disableDepthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		disableDepthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		disableDepthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		disableDepthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		disableDepthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_d3d->CreateDepthStencilState(&disableDepthStencilDesc, &m_disableDepthState);
		if (FAILED(hRes))
			return FALSE;
		D3D10_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.MaxDepth = 1.0F;
		vp.TopLeftX = INT(x);
		vp.TopLeftY = INT(y);
		vp.Width = UINT(cx);
		vp.Height = UINT(cy);
		m_d3d->RSSetViewports(1, &vp);
		FLOAT fov = (FLOAT)D3DX_PI / 4.0f;
		FLOAT aspect = (FLOAT)cx / (FLOAT)cy;
		D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fov, aspect, 1000.0F, 0.1F);
		D3DXMatrixIdentity(&m_worldMatrix);
		D3DXMatrixOrthoLH(&m_orthoMatrix, (FLOAT)cx, (FLOAT)cy, 1000.0F, 0.1F);
		return TRUE;
	}
	BOOL DX10::ResizeView()
	{
		ASSERT(m_d3d && m_swap);
		LPVOID val = NULL;
		m_d3d->OMSetRenderTargets(1, (ID3D10RenderTargetView**)&val, NULL);
		m_renderView.Release();
		HRESULT hRes = m_swap->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D10Texture2D> backBuffer;
		hRes = m_swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX10::BeginScene()
	{
		ASSERT(m_d3d);
		FLOAT color[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
		m_d3d->ClearRenderTargetView(m_renderView, color);
		m_d3d->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0F, 0);
	}
	void DX10::EndScene()
	{
		ASSERT(m_swap);
		m_swap->Present(0, 0);//越快越好
	}
	void DX10::AllowDepth(BOOL allow)
	{
		ASSERT(m_d3d);
		if (allow)
			m_d3d->OMSetDepthStencilState(m_depthStencilState, 1);
		else
			m_d3d->OMSetDepthStencilState(m_disableDepthState, 1);
	}
	ID3D10Device1*	DX10::GetD3D() const
	{
		return m_d3d;
	}
	HWND DX10::GetHWND() const
	{
		return m_hWND;
	}
	SIZE DX10::GetSize() const
	{
		return m_size;
	}
	D3DXMATRIX DX10::GetProjectionMatrix()
	{
		return m_projectionMatrix;
	}
	D3DXMATRIX DX10::GetWorldMatrix()
	{
		return m_worldMatrix;
	}
	D3DXMATRIX DX10::GetOrthoMatrix()
	{
		return m_orthoMatrix;
	}
}
