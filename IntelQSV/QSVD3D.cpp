#include "stdafx.h"
#include "QSVD3D.h"

namespace QSV
{
	QSVD3D9::QSVD3D9()
		:m_handle(NULL),
		m_resetToken(0)
	{

	}
	QSVD3D9::~QSVD3D9()
	{

	}
	mfxStatus QSVD3D9::Initialize(mfxHDL hWindow, UINT adapter)
	{
		HRESULT hRes = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_d3d9);
		if (!m_d3d9 || FAILED(hRes))
			return MFX_ERR_DEVICE_FAILED;
		memset(&m_d3dPP, 0, sizeof(m_d3dPP));
		m_d3dPP.Windowed = TRUE;
		m_d3dPP.hDeviceWindow = (HWND)hWindow;
		m_d3dPP.Flags = D3DPRESENTFLAG_VIDEO;
		m_d3dPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_d3dPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		m_d3dPP.BackBufferCount = 1;
		m_d3dPP.BackBufferFormat = D3DFMT_A8R8G8B8;
		if (hWindow)
		{
			RECT s;
			GetClientRect((HWND)hWindow, &s);
			INT x = GetSystemMetrics(SM_CXSCREEN);
			INT y = GetSystemMetrics(SM_CYSCREEN);
			m_d3dPP.BackBufferWidth = min(s.right - s.left, x);
			m_d3dPP.BackBufferHeight = min(s.bottom - s.top, y);
		}
		else
		{
			m_d3dPP.BackBufferWidth = GetSystemMetrics(SM_CYSCREEN);
			m_d3dPP.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		m_d3dPP.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		hRes = m_d3d9->CreateDeviceEx(adapter,
			D3DDEVTYPE_HAL,
			(HWND)hWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&m_d3dPP,
			NULL,
			&m_d3dd9);
		if (FAILED(hRes))
			return MFX_ERR_NULL_PTR;
		if (hWindow)
		{
			hRes = m_d3dd9->ResetEx(&m_d3dPP, NULL);
			if (FAILED(hRes))
				return MFX_ERR_UNDEFINED_BEHAVIOR;
			hRes = m_d3dd9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			if (FAILED(hRes))
				return MFX_ERR_UNDEFINED_BEHAVIOR;
		}

		hRes = DXVA2CreateDirect3DDeviceManager9(&m_resetToken, &m_manager);
		if (FAILED(hRes))
			return MFX_ERR_NULL_PTR;
		hRes = m_manager->ResetDevice(m_d3dd9, m_resetToken);
		if (FAILED(hRes))
			return MFX_ERR_UNDEFINED_BEHAVIOR;
		return MFX_ERR_NONE;
	}

	mfxHDL QSVD3D9::GetHandle(mfxHandleType type)
	{
		if (MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9 == type)
		{
			return (mfxHDL)m_manager;
		}
		return NULL;
	}

	mfxStatus QSVD3D9::Reset()
	{
		HRESULT hRes = NO_ERROR;
		MSDK_CHECK_POINTER(m_d3dd9, MFX_ERR_NULL_PTR);
		if (m_d3dPP.Windowed)
		{
			RECT s;
			GetClientRect((HWND)m_d3dPP.hDeviceWindow, &s);
			INT x = GetSystemMetrics(SM_CXSCREEN);
			INT y = GetSystemMetrics(SM_CYSCREEN);
			m_d3dPP.BackBufferWidth = min(s.right - s.left, x);
			m_d3dPP.BackBufferHeight = min(s.bottom - s.top, y);
		}
		else
		{
			m_d3dPP.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
			m_d3dPP.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		D3DPRESENT_PARAMETERS d3dpp = m_d3dPP;
		hRes = m_d3dd9->ResetEx(&d3dpp, NULL);
		if (FAILED(hRes))
			return MFX_ERR_UNDEFINED_BEHAVIOR;
		hRes = m_manager->ResetDevice(m_d3dd9, m_resetToken);
		if (FAILED(hRes))
			return MFX_ERR_UNDEFINED_BEHAVIOR;
		return MFX_ERR_NONE;
	}

	void QSVD3D9::Close()
	{
		m_manager.Release();
		m_d3d9.Release();
		m_d3dd9.Release();
	}
	DWORD QSVD3D9::GetUsage() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	/*QSVD3D11::QSVD3D11()
	{
	}
	QSVD3D11::~QSVD3D11()
	{
		Close();
	}
	mfxStatus QSVD3D11::Initialize(mfxHDL hWindow, UINT adapter)
	{
		static const D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};
		D3D_FEATURE_LEVEL pFeatureLevelsOut;
		mfxStatus status = MFX_ERR_NONE;
		HRESULT hRes = S_OK;
		hRes = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)(&m_pDXGIFactory));
		if (FAILED(hRes))
			return MFX_ERR_DEVICE_FAILED;
		hRes = m_pDXGIFactory->EnumAdapters(adapter, &m_pAdapter);
		if (FAILED(hRes))
			return MFX_ERR_DEVICE_FAILED;
		hRes = D3D11CreateDevice(m_pAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			0,
			featureLevels,
			MSDK_ARRAY_LEN(featureLevels),
			D3D11_SDK_VERSION,
			&m_pD3D11Device,
			&pFeatureLevelsOut,
			&m_pD3D11Ctx);
		if (FAILED(hRes))
			return MFX_ERR_DEVICE_FAILED;
		m_pDX11VideoDevice = m_pD3D11Device;
		m_pVideoContext = m_pD3D11Ctx;
		MSDK_CHECK_POINTER(m_pDX11VideoDevice, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_pVideoContext, MFX_ERR_NULL_PTR);
		TinyComQIPtr<ID3D10Multithread> mt(m_pVideoContext);
		if (mt)
			mt->SetMultithreadProtected(TRUE);
		else
			return MFX_ERR_DEVICE_FAILED;
		return status;
	}
	mfxHDL QSVD3D11::GetHandle(mfxHandleType type)
	{
		if (MFX_HANDLE_D3D11_DEVICE == type)
		{
			return (mfxHDL)m_pD3D11Device;
		}
		return NULL;
	}
	mfxStatus QSVD3D11::Reset()
	{
		return MFX_ERR_NONE;
	}
	void QSVD3D11::Close()
	{
		Reset();
	}*/
}