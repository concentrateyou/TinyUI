#include "stdafx.h"
//#include "DX9.h"
//#include "DX9Graphics2D.h"
//#include "Common/TinyLogging.h"
//
//namespace DXFramework
//{
//	DX9::DX9()
//		:m_hWND(NULL),
//		m_bActive(FALSE)
//	{
//		ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
//	}
//	DX9::~DX9()
//	{
//
//	}
//	BOOL DX9::Initialize(HWND hWND, const TinySize& size)
//	{
//		return Initialize(hWND, size.cx, size.cy);
//	}
//	BOOL DX9::Initialize(HWND hWND, INT cx, INT cy)
//	{
//		if (!IsWindow(hWND))
//			return FALSE;
//		m_d3d9.Release();
//		m_d3d9.Attach(Direct3DCreate9(D3D_SDK_VERSION));
//		if (m_d3d9 == NULL)
//			return FALSE;
//		D3DDISPLAYMODE displayMode;
//		HRESULT hRes = m_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
//		D3DMULTISAMPLE_TYPE multiType = D3DMULTISAMPLE_NONE;
//		if (m_d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
//			D3DDEVTYPE_HAL, displayMode.Format, TRUE,
//			D3DMULTISAMPLE_4_SAMPLES,
//			NULL) == D3D_OK)
//		{
//			multiType = D3DMULTISAMPLE_4_SAMPLES;
//		}
//		m_d3dpp.BackBufferWidth = cx;
//		m_d3dpp.BackBufferHeight = cy;
//		m_d3dpp.BackBufferFormat = displayMode.Format;
//		m_d3dpp.BackBufferCount = 1;
//		m_d3dpp.MultiSampleType = multiType;
//		m_d3dpp.MultiSampleQuality = 0;
//		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
//		m_d3dpp.hDeviceWindow = hWND;
//		m_d3dpp.Windowed = TRUE;
//		m_d3dpp.EnableAutoDepthStencil = TRUE;
//		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
//		m_d3dpp.Flags = 0;
//		m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//		m_d3dd9.Release();
//		hRes = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &m_d3dpp, &m_d3dd9);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] CreateDevice:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] CreateDevice:" << hRes;
//			return FALSE;
//		}
//		D3DVIEWPORT9 vp;
//		ZeroMemory(&vp, sizeof(vp));
//		vp.X = 0;
//		vp.Y = 0;
//		vp.Width = cx;
//		vp.Height = cy;
//		vp.MinZ = 0.0F;
//		vp.MaxZ = 1.0F;
//		hRes = m_d3dd9->SetViewport(&vp);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetViewport:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetViewport:" << hRes;
//			return FALSE;
//		}
//		this->SetMatrixs(m_size);
//		hRes = m_d3dd9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_CULLMODE:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_CULLMODE:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_ZENABLE, TRUE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_ZENABLE:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ZENABLE:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_ALPHABLENDENABLE:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_SRCBLEND:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_SRCBLEND:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_DESTBLEND:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_DESTBLEND:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_BLENDOP:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_BLENDOP:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetRenderState(D3DRS_LIGHTING, FALSE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetRenderState D3DRS_LIGHTING:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetRenderState D3DRS_LIGHTING:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_COLORARG1:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLORARG1:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_COLORARG2:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLORARG2:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_COLOROP:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_COLOROP:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAARG1:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAARG2:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAARG2:" << hRes;
//			return FALSE;
//		}
//		hRes = m_d3dd9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
//		if (hRes != S_OK)
//		{
//			TRACE("[Initialize] SetTextureStageState D3DTSS_ALPHAOP:%d\n", hRes);
//			LOG(ERROR) << "[Initialize] SetTextureStageState D3DTSS_ALPHAOP:" << hRes;
//			return FALSE;
//		}
//		m_hWND = hWND;
//		m_size.cx = cx;
//		m_size.cy = cy;
//		m_bActive = TRUE;
//		return TRUE;
//	}
//	BOOL DX9::CheckReason(HRESULT val)
//	{
//		if (IsEmpty())
//			return FALSE;
//		HRESULT hRes = m_d3dd9->TestCooperativeLevel();
//		return val == hRes;
//	}
//	BOOL DX9::Present()
//	{
//		if (IsEmpty())
//			return FALSE;
//		HRESULT hRes = m_d3dd9->Present(NULL, NULL, NULL, NULL);
//		if (hRes != S_OK)
//		{
//			TRACE("[Present] Present:%d\n", hRes);
//			LOG(ERROR) << "[Present] Present:" << hRes;
//			if (hRes == D3DERR_DEVICELOST)
//			{
//				m_bActive = FALSE;
//			}
//			return FALSE;
//		}
//		return TRUE;
//	}
//	BOOL DX9::Resize(const TinySize& size)
//	{
//		return Resize(size.cx, size.cy);
//	}
//	BOOL DX9::Resize(INT cx, INT cy)
//	{
//		if (IsEmpty())
//			return FALSE;
//		m_d3dpp.BackBufferWidth = cx;
//		m_d3dpp.BackBufferHeight = cy;
//		HRESULT hRes = m_d3dd9->Reset(&m_d3dpp);
//		if (hRes != S_OK)
//			return FALSE;
//		m_size.cx = cx;
//		m_size.cy = cy;
//		this->SetViewport(TinyPoint(0, 0), m_size);
//		this->SetMatrixs(m_size);
//		return TRUE;
//	}
//	BOOL DX9::SetViewport(const TinyPoint& pos, const TinySize& size)
//	{
//		if (IsEmpty())
//			return FALSE;
//		D3DVIEWPORT9 vp;
//		ZeroMemory(&vp, sizeof(vp));
//		vp.X = pos.x;
//		vp.Y = pos.y;
//		vp.Width = size.cx;
//		vp.Height = size.cy;
//		vp.MinZ = 0.0F;
//		vp.MaxZ = 1.0F;
//		HRESULT hRes = m_d3dd9->SetViewport(&vp);
//		if (hRes != S_OK)
//		{
//			TRACE("[SetViewport] SetViewport:%d\n", hRes);
//			LOG(ERROR) << "[SetViewport] SetViewport:" << hRes;
//			return FALSE;
//		}
//		return TRUE;
//	}
//	HWND DX9::GetHWND() const
//	{
//		return m_hWND;
//	}
//	BOOL DX9::IsEmpty() const
//	{
//		if (m_d3d9 != NULL &&
//			m_d3dd9 != NULL)
//			return FALSE;
//		return TRUE;
//	}
//	BOOL DX9::IsActive() const
//	{
//		return m_bActive;
//	}
//	IDirect3DDevice9*	DX9::GetD3D() const
//	{
//		return m_d3dd9;
//	}
//	TinySize DX9::GetSize() const
//	{
//		return m_size;
//	}
//	void DX9::SetMatrixs(const TinySize& size)
//	{
//		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
//		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
//		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 0.1F, 1000.0F);//Projection
//		m_matrixs[1] = XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0F, 0.0F, -10.0F)),
//			XMLoadFloat3(&XMFLOAT3(0.0F, 0.0F, 0.0F)),
//			XMLoadFloat3(&XMFLOAT3(0.0F, 1.0F, 0.0F)));//View
//		m_matrixs[2] = XMMatrixIdentity();//World
//	}
//	XMMATRIX* DX9::GetMatrixs()
//	{
//		return m_matrixs;
//	}
//}
