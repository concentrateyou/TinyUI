#include "stdafx.h"
#include "DX9.h"
#include "DX9RenderView.h"
#include "Common/TinyLogging.h"

namespace DXFramework
{
	DX9::DX9()
		:m_hWND(NULL),
		m_render2D(NULL)
	{

	}
	DX9::~DX9()
	{

	}
	BOOL DX9::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_d3d9.Attach(Direct3DCreate9(D3D_SDK_VERSION));
		if (m_d3d9 == NULL)
			return FALSE;
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferWidth = cx;
		d3dpp.BackBufferHeight = cy;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hWND;
		d3dpp.Windowed = TRUE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		HRESULT hRes = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_d3dd9);
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
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		if (hRes != S_OK)
			return FALSE;
		m_background2D.Reset(new DX9RenderView(*this));
		if (!m_background2D->Create())
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
	BOOL DX9::ResizeView(INT cx, INT cy)
	{
		if (IsEmpty())
			return FALSE;
		if (!m_background2D->Resize())
			return FALSE;
		this->SetViewport(TinyPoint(0, 0), m_background2D->GetSize());
		this->SetMatrixs(m_background2D->GetSize());
		return TRUE;
	}
	BOOL DX9::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (IsEmpty())
			return FALSE;
		D3DVIEWPORT9 viewport;
		viewport.X = pos.x;
		viewport.Y = pos.y;
		viewport.Width = size.cx;
		viewport.Height = size.cy;
		viewport.MinZ = 0.0F;
		viewport.MaxZ = 1.0F;
		m_d3dd9->SetViewport(&viewport);
		return TRUE;
	}
	HWND DX9::GetHWND() const
	{
		return m_hWND;
	}
	BOOL DX9::IsEmpty() const
	{
		if (m_d3d9 != NULL &&
			m_d3dd9 != NULL)
			return FALSE;
		return TRUE;
	}
	IDirect3DDevice9*	DX9::GetD3D() const
	{
		return m_d3dd9;
	}
	void DX9::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		D3DXMatrixPerspectiveFovLH(&m_matrixs[0], fov, aspect, 1000.0F, 0.1F);//Projection
		D3DXMatrixLookAtLH(&m_matrixs[1], &D3DXVECTOR3(0.0F, 0.0F, -10.0F), &D3DXVECTOR3(0.0F, 0.0F, 0.0F), &D3DXVECTOR3(0.0F, 1.0F, 0.0F));//View
		D3DXMatrixIdentity(&m_matrixs[2]);
	}
	D3DXMATRIX* DX9::GetMatrixs()
	{
		return m_matrixs;
	}
	DX9RenderView* DX9::GetRender2D() const
	{
		return m_render2D;
	}
	void DX9::SetRenderTexture2D(DX9RenderView* render2D)
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
}
