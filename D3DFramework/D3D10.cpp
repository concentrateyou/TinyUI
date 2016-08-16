#include "stdafx.h"
#include "D3D10.h"

namespace DX
{
	D3D10::D3D10()
		:m_fovY(0.0F),
		m_aspect(0.0F),
		m_nearZ(0.0F),
		m_farZ(0.0F),
		m_cx(0),
		m_cy(0)
	{
		ZeroMemory(&m_projectionMatrix, sizeof(m_projectionMatrix));
		ZeroMemory(&m_worldMatrix, sizeof(m_worldMatrix));
		ZeroMemory(&m_orthoMatrix, sizeof(m_orthoMatrix));
	}


	D3D10::~D3D10()
	{

	}

	BOOL D3D10::Initialize(HWND hWND, INT cx, INT cy, FLOAT nearZ, FLOAT farZ)
	{
		m_cx = cx;
		m_cy = cy;
		m_nearZ = nearZ;
		m_farZ = farZ;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = cx;
		swapChainDesc.BufferDesc.Height = cy;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWND;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		HRESULT hRes = S_OK;
		hRes = D3D10CreateDeviceAndSwapChain1(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D10Texture2D> backBufferPtr;
		hRes = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
		if (FAILED(hRes))
			return FALSE;
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
		hRes = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(hRes))
			return FALSE;
		D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = TRUE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(hRes))
			return FALSE;
		m_device->OMSetDepthStencilState(m_depthStencilState, 1);
		D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hRes = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hRes))
			return FALSE;
		m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		D3D10_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.CullMode = D3D10_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0F;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.FillMode = D3D10_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.ScissorEnable = FALSE;
		rasterDesc.SlopeScaledDepthBias = 0.0F;
		hRes = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		if (FAILED(hRes))
			return FALSE;
		m_device->RSSetState(m_rasterState);
		D3D10_VIEWPORT viewport;
		viewport.Width = cx;
		viewport.Height = cy;
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		m_device->RSSetViewports(1, &viewport);
		m_fovY = (FLOAT)D3DX_PI / 4.0f;
		m_aspect = (FLOAT)cx / (FLOAT)cy;
		D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, m_fovY, m_aspect, m_nearZ, m_farZ);
		D3DXMatrixIdentity(&m_worldMatrix);
		D3DXMatrixOrthoLH(&m_orthoMatrix, (FLOAT)cx, (FLOAT)cy, m_nearZ, m_farZ);
		D3D10_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
		depthDisabledStencilDesc.DepthEnable = FALSE;
		depthDisabledStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = TRUE;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	ID3D10Device1* D3D10::GetDevice() const
	{
		return m_device;
	}
}
