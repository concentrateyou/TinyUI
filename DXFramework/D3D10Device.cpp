#include "stdafx.h"
#include "D3D10Device.h"

namespace DXFramework
{
	D3D10Device::D3D10Device()
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


	D3D10Device::~D3D10Device()
	{

	}

	BOOL D3D10Device::Initialize(HWND hWND, INT cx, INT cy, FLOAT nearZ, FLOAT farZ)
	{
		m_cx = cx;
		m_cy = cy;
		m_nearZ = nearZ;
		m_farZ = farZ;
		DXGI_SWAP_CHAIN_DESC dscd;
		ZeroMemory(&dscd, sizeof(dscd));
		dscd.BufferCount = 1;
		dscd.BufferDesc.Width = cx;
		dscd.BufferDesc.Height = cy;
		dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dscd.BufferDesc.RefreshRate.Numerator = 0;
		dscd.BufferDesc.RefreshRate.Denominator = 1;
		dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dscd.OutputWindow = hWND;
		dscd.SampleDesc.Count = 1;
		dscd.SampleDesc.Quality = 0;
		dscd.Windowed = TRUE;
		dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dscd.Flags = 0;
		HRESULT hRes = S_OK;
		hRes = D3D10CreateDeviceAndSwapChain1(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &dscd, &m_swapChain, &m_device);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D10Texture2D> backBufferPtr;
		hRes = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
		if (FAILED(hRes))
			return FALSE;
		D3D10_TEXTURE2D_DESC dtd;
		ZeroMemory(&dtd, sizeof(dtd));
		dtd.Width = cx;
		dtd.Height = cy;
		dtd.MipLevels = 1;
		dtd.ArraySize = 1;
		dtd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dtd.SampleDesc.Count = 1;
		dtd.SampleDesc.Quality = 0;
		dtd.Usage = D3D10_USAGE_DEFAULT;
		dtd.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		dtd.CPUAccessFlags = 0;
		dtd.MiscFlags = 0;
		hRes = m_device->CreateTexture2D(&dtd, NULL, &m_depthStencilBuffer);
		if (FAILED(hRes))
			return FALSE;
		D3D10_DEPTH_STENCIL_DESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.DepthEnable = TRUE;
		ddsd.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		ddsd.DepthFunc = D3D10_COMPARISON_LESS;
		ddsd.StencilEnable = TRUE;
		ddsd.StencilReadMask = 0xFF;
		ddsd.StencilWriteMask = 0xFF;
		ddsd.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		ddsd.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		ddsd.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		ddsd.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		ddsd.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		ddsd.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		ddsd.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		ddsd.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_device->CreateDepthStencilState(&ddsd, &m_depthStencilState);
		if (FAILED(hRes))
			return FALSE;
		D3D10_DEPTH_STENCIL_VIEW_DESC ddsvd;
		ZeroMemory(&ddsvd, sizeof(ddsvd));
		ddsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ddsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		ddsvd.Texture2D.MipSlice = 0;
		hRes = m_device->CreateDepthStencilView(m_depthStencilBuffer, &ddsvd, &m_depthStencilView);
		if (FAILED(hRes))
			return FALSE;
		m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		D3D10_RASTERIZER_DESC drd;
		drd.AntialiasedLineEnable = FALSE;
		drd.CullMode = D3D10_CULL_BACK;
		drd.DepthBias = 0;
		drd.DepthBiasClamp = 0.0F;
		drd.DepthClipEnable = TRUE;
		drd.FillMode = D3D10_FILL_SOLID;
		drd.FrontCounterClockwise = FALSE;
		drd.MultisampleEnable = FALSE;
		drd.ScissorEnable = FALSE;
		drd.SlopeScaledDepthBias = 0.0F;
		hRes = m_device->CreateRasterizerState(&drd, &m_rasterState);
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
		//单位法向量
		D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, m_fovY, m_aspect, m_nearZ, m_farZ);
		//单位矩阵
		D3DXMatrixIdentity(&m_worldMatrix);
		//左手坐标系矩阵
		D3DXMatrixOrthoLH(&m_orthoMatrix, (FLOAT)cx, (FLOAT)cy, m_nearZ, m_farZ);
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.DepthEnable = FALSE;
		ddsd.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		ddsd.DepthFunc = D3D10_COMPARISON_LESS;
		ddsd.StencilEnable = TRUE;
		ddsd.StencilReadMask = 0xFF;
		ddsd.StencilWriteMask = 0xFF;
		ddsd.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		ddsd.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		ddsd.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		ddsd.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		ddsd.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		ddsd.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		ddsd.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		ddsd.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		hRes = m_device->CreateDepthStencilState(&ddsd, &m_disableDepthStencilState);
		if (FAILED(hRes))
			return FALSE;
		//设置成2D
		m_device->OMSetDepthStencilState(m_disableDepthStencilState, 1);
		return TRUE;
	}
	void D3D10Device::AllowDepth(BOOL bEnable)
	{
		ASSERT(m_device);
		if (bEnable)
		{
			m_device->OMSetDepthStencilState(m_depthStencilState, 1);
		}
		else
		{
			m_device->OMSetDepthStencilState(m_disableDepthStencilState, 1);
		}
	}
	ID3D10Device1* D3D10Device::GetDevice() const
	{
		return m_device;
	}
	void D3D10Device::BeginScene(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
	{
		FLOAT color[4];
		color[0] = r;
		color[1] = g;
		color[2] = g;
		color[3] = a;
		m_device->ClearRenderTargetView(m_renderTargetView, color);
		//1.0F最大清除深度
		m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0F, 0);
	}
	void D3D10Device::EndScene()
	{
		ASSERT(m_swapChain);
		//0,0越快越好
		m_swapChain->Present(0, 0);
	}

	void D3D10Device::GetProjectionMatrix(D3DXMATRIX& m)
	{
		m = m_projectionMatrix;
	}
	void D3D10Device::GetWorldMatrix(D3DXMATRIX& m)
	{
		m = m_worldMatrix;
	}
	void D3D10Device::GetOrthoMatrix(D3DXMATRIX& m)
	{
		m = m_orthoMatrix;
	}
}
