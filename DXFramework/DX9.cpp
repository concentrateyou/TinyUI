#include "stdafx.h"
#include "DX9.h"
#include "Common/TinyLogging.h"

namespace DXFramework
{
	DX9::DX9()
		:m_hWND(NULL)
	{

	}
	DX9::~DX9()
	{

	}
	BOOL DX9::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		HRESULT hRes = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_d3d9);
		if (hRes != S_OK)
			return FALSE;
		D3DPRESENT_PARAMETERS d3dpp = { 0 };
		d3dpp.BackBufferWidth = cx;
		d3dpp.BackBufferHeight = cy;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hWND;
		d3dpp.Windowed = FALSE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		hRes = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_d3dd9);
		if (hRes != S_OK)
			return FALSE;
		ZeroMemory(&m_viewPort, sizeof(m_viewPort));
		m_viewPort.X = 0;
		m_viewPort.Y = 0;
		m_viewPort.Width = cx;
		m_viewPort.Height = cy;
		m_viewPort.MinZ = 0.0F;
		m_viewPort.MaxZ = 1.0F;
		hRes = m_d3dd9->SetViewport(&m_viewPort);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetRenderState(D3DRS_ZENABLE, TRUE);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	BOOL DX9::Present()
	{
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_d3dd9->Present(NULL, NULL, NULL, NULL);
		return SUCCEEDED(hRes);
	}
	HWND DX9::GetHWND() const
	{
		return m_hWND;
	}
	BOOL DX9::IsEmpty() const
	{
		if (m_d3d9 != NULL &&
			m_d3dd9 != NULL)
			return TRUE;
		return FALSE;
	}
	IDirect3DDevice9*	DX9::GetD3D() const
	{
		return m_d3dd9;
	}
	void DX9::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		D3DXMatrixPerspectiveFovLH(&m_matrixs[0], fov, aspect, 1000.0F, 0.1F);
		D3DXMatrixIdentity(m_matrixs[1]);
		D3DXMatrixOrthoLH(&m_matrixs[2], (FLOAT)size.cx, (FLOAT)size.cy, 1000.0F, 0.1F);
	}
	D3DXMATRIX* DX9::GetMatrixs()
	{
		return m_matrixs;
	}
}
