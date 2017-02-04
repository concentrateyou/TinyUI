#include "stdafx.h"
#include "QSVH264.h"

namespace QSV
{
	QSVH264::QSVH264()
	{
	}

	QSVH264::~QSVH264()
	{
	}

	BOOL QSVH264::Initialize()
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY | MFX_IMPL_VIA_D3D11;
		mfxVersion ver = { { 1 , 1 } };
		mfxStatus sts = MFXInit(impl, &ver, &m_session);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		return TRUE;
	}
}