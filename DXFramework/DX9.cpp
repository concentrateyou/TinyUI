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
		return TRUE;
	}
	BOOL DX9::Render()
	{
		if (IsEmpty())
			return FALSE;
		return TRUE;
		//HRESULT hRes =  m_d3dd9->BeginScene();
		//if (hRes != S_OK)
		//	return FALSE;
		//hRes = m_d3dd9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0F, 1.0F, 0.0F, 1.0F), 1.0F, 0);
		//if (hRes != S_OK)
		//	return FALSE;
		//hRes = m_d3dd9->EndScene();
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
}
