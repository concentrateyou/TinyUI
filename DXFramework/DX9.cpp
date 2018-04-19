#include "stdafx.h"
#include "DX9.h"
#include "DX9RenderView.h"
#include "Common/TinyLogging.h"

namespace DXFramework
{
	DX9::DX9()
		:m_hWND(NULL),
		m_render2D(NULL),
		m_bActive(FALSE)
	{

	}
	DX9::~DX9()
	{

	}
	BOOL DX9::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_size.cx = cx;
		m_size.cy = cy;
		m_d3d9.Release();
		m_d3d9.Attach(Direct3DCreate9(D3D_SDK_VERSION));
		if (m_d3d9 == NULL)
			return FALSE;
		D3DDISPLAYMODE displayMode;
		HRESULT hRes = m_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
		D3DMULTISAMPLE_TYPE multiType = D3DMULTISAMPLE_NONE;
		if (m_d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, displayMode.Format, TRUE,
			D3DMULTISAMPLE_4_SAMPLES,
			NULL) == D3D_OK)
		{
			multiType = D3DMULTISAMPLE_4_SAMPLES;
		}
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferWidth = cx;
		d3dpp.BackBufferHeight = cy;
		d3dpp.BackBufferFormat = displayMode.Format;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = multiType;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hWND;
		d3dpp.Windowed = TRUE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dd9.Release();
		hRes = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &d3dpp, &m_d3dd9);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] CreateDevice:%d\n", hRes);
			LOG(ERROR) << "[Initialize] CreateDevice:" << hRes;
			return FALSE;
		}
		D3DVIEWPORT9 vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.X = 0;
		vp.Y = 0;
		vp.Width = cx;
		vp.Height = cy;
		vp.MinZ = 0.0F;
		vp.MaxZ = 1.0F;
		hRes = m_d3dd9->SetViewport(&vp);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetViewport:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetViewport:" << hRes;
			return FALSE;
		}
		this->SetMatrixs(m_size);
		hRes = m_d3dd9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_CULLMODE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_CULLMODE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_ZENABLE, TRUE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_ZENABLE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ZENABLE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_SRCBLEND:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_SRCBLEND:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_DESTBLEND:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_DESTBLEND:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_BLENDOP:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_BLENDOP:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_LIGHTING, FALSE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_LIGHTING:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_LIGHTING:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_COLORARG1:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLORARG1:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_COLORARG2:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLORARG2:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_COLOROP:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLOROP:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAARG2:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAARG2:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAOP:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAOP:" << hRes;
			return FALSE;
		}
		m_background2D.Reset(new DX9RenderView(*this));
		if (!m_background2D->Create())
			return FALSE;
		m_bActive = TRUE;
		return TRUE;
	}
	BOOL DX9::Reset()
	{
		D3DDISPLAYMODE displayMode;
		HRESULT hRes = m_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
		D3DMULTISAMPLE_TYPE multiType = D3DMULTISAMPLE_NONE;
		if (m_d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, displayMode.Format, TRUE,
			D3DMULTISAMPLE_4_SAMPLES,
			NULL) == D3D_OK)
		{
			multiType = D3DMULTISAMPLE_4_SAMPLES;
		}
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferWidth = m_size.cx;
		d3dpp.BackBufferHeight = m_size.cy;
		d3dpp.BackBufferFormat = displayMode.Format;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = multiType;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = m_hWND;
		d3dpp.Windowed = TRUE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dd9.Release();
		hRes = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_d3dd9);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] CreateDevice:%d\n", hRes);
			LOG(ERROR) << "[Initialize] CreateDevice:" << hRes;
			return FALSE;
		}
		D3DVIEWPORT9 vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.X = 0;
		vp.Y = 0;
		vp.Width = m_size.cx;
		vp.Height = m_size.cy;
		vp.MinZ = 0.0F;
		vp.MaxZ = 1.0F;
		hRes = m_d3dd9->SetViewport(&vp);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetViewport:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetViewport:" << hRes;
			return FALSE;
		}
		this->SetMatrixs(m_size);
		hRes = m_d3dd9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_CULLMODE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_CULLMODE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_ZENABLE, TRUE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_ZENABLE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ZENABLE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_SRCBLEND:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_SRCBLEND:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetRenderState D3DRS_DESTBLEND:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_DESTBLEND:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_COLORARG1:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLORARG1:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_COLOROP:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLOROP:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:" << hRes;
			return FALSE;
		}
		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		if (hRes != S_OK)
		{
			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAOP:%d\n", hRes);
			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAOP:" << hRes;
			return FALSE;
		}
		m_background2D.Reset(new DX9RenderView(*this));
		if (!m_background2D->Create())
			return FALSE;
		m_bActive = TRUE;
		return TRUE;
	}
	BOOL DX9::CheckReason(HRESULT val)
	{
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_d3dd9->TestCooperativeLevel();
		return val == hRes;
	}
	BOOL DX9::Present()
	{
		if (IsEmpty())
			return FALSE;
		HRESULT hRes = m_d3dd9->Present(NULL, NULL, NULL, NULL);
		if (hRes != S_OK)
		{
			TRACE("[Present] Present:%d\n", hRes);
			LOG(ERROR) << "[Present] Present:" << hRes;
			if (hRes == D3DERR_DEVICELOST)
			{
				m_bActive = FALSE;
			}
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX9::ResizeView(INT cx, INT cy)
	{
		if (IsEmpty())
			return FALSE;
		if (!m_background2D->Resize())
			return FALSE;
		m_size.cx = cx;
		m_size.cy = cy;
		this->SetViewport(TinyPoint(0, 0), m_background2D->GetSize());
		this->SetMatrixs(m_background2D->GetSize());
		return TRUE;
	}
	BOOL DX9::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		if (IsEmpty())
			return FALSE;
		D3DVIEWPORT9 vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.X = pos.x;
		vp.Y = pos.y;
		vp.Width = size.cx;
		vp.Height = size.cy;
		vp.MinZ = 0.0F;
		vp.MaxZ = 1.0F;
		HRESULT hRes = m_d3dd9->SetViewport(&vp);
		if (hRes != S_OK)
		{
			TRACE("[SetViewport] SetViewport:%d\n", hRes);
			LOG(ERROR) << "[SetViewport] SetViewport:" << hRes;
			return FALSE;
		}
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
	BOOL DX9::IsActive() const
	{
		return m_bActive;
	}
	IDirect3DDevice9*	DX9::GetD3D() const
	{
		return m_d3dd9;
	}
	TinySize DX9::GetSize() const
	{
		return m_size;
	}
	void DX9::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		D3DXMatrixPerspectiveFovLH(&m_matrixs[0], fov, aspect, 1000.0F, 0.1F);//Projection
		D3DXMatrixLookAtLH(&m_matrixs[1],
			&D3DXVECTOR3(0.0F, 0.0F, -10.0F),
			&D3DXVECTOR3(0.0F, 0.0F, 0.0F),
			&D3DXVECTOR3(0.0F, 1.0F, 0.0F));//View
		D3DXMatrixIdentity(&m_matrixs[2]);//World
	}
	D3DXMATRIX* DX9::GetMatrixs()
	{
		return m_matrixs;
	}
	DX9RenderView* DX9::GetRenderView() const
	{
		return m_render2D;
	}
	void DX9::SetRenderView(DX9RenderView* render2D)
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
