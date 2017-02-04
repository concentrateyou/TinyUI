#include "stdafx.h"
#include "QSVDX11.h"

namespace QSV
{
	QSVDX11::QSVDX11()
	{

	}

	QSVDX11::~QSVDX11()
	{
	}

	BOOL QSVDX11::Initialize(HWND hWND)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY | MFX_IMPL_VIA_D3D11;
		mfxVersion ver = { { 1 , 1 } };
		mfxStatus status = MFXInit(impl, &ver, &m_session);
		if (status != MFX_ERR_NONE)
			return FALSE;
		HRESULT hRes = S_OK;
		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3
		};
		status = MFXQueryIMPL(m_session, &impl);
		if (status != MFX_ERR_NONE)
			return FALSE;
		impl = MFX_IMPL_BASETYPE(impl);
		switch (impl)
		{
		case MFX_IMPL_HARDWARE:
		case MFX_IMPL_HARDWARE_ANY:
		case MFX_IMPL_HARDWARE2:
		case MFX_IMPL_HARDWARE3:
		case MFX_IMPL_HARDWARE4:
		{
			hRes = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)&m_dxgiFactory2);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IDXGIAdapter> adapter;
			//hres = m_dxgiFactory2->EnumAdapters(adapterNum, &adapter);
		}
		break;
		}
		return TRUE;
	}
}