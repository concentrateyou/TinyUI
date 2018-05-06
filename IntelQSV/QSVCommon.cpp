#include "stdafx.h"
#include "QSVCommon.h"
#include <ppl.h>

namespace QSV
{
	D3DFORMAT ConvertMFXFourccToD3DFormat(mfxU32 fourcc)
	{
		switch (fourcc)
		{
		case MFX_FOURCC_NV12:
			return D3DFMT_NV12;
		case MFX_FOURCC_YV12:
			return D3DFMT_YV12;
		case MFX_FOURCC_NV16:
			return D3DFMT_NV16;
		case MFX_FOURCC_YUY2:
			return D3DFMT_YUY2;
		case MFX_FOURCC_RGB3:
			return D3DFMT_R8G8B8;
		case MFX_FOURCC_RGB4:
			return D3DFMT_A8R8G8B8;
		case MFX_FOURCC_P8:
			return D3DFMT_P8;
		case MFX_FOURCC_P010:
			return D3DFMT_P010;
		case MFX_FOURCC_P210:
			return D3DFMT_P210;
		case MFX_FOURCC_A2RGB10:
			return D3DFMT_A2R10G10B10;
		case MFX_FOURCC_ABGR16:
		case MFX_FOURCC_ARGB16:
			return D3DFMT_A16B16G16R16;
		case MFX_FOURCC_IMC3:
			return D3DFMT_IMC3;
		default:
			return D3DFMT_UNKNOWN;
		}
	}

	mfxU32 GetIntelAdapter(mfxSession session)
	{
		mfxU32  adapters = 0;
		mfxIMPL impl = MFX_IMPL_SOFTWARE;
		if (session)
		{
			MFXQueryIMPL(session, &impl);
		}
		else
		{
			mfxSession auxSession;
			memset(&auxSession, 0, sizeof(auxSession));
			mfxVersion ver = { 1, 1 };
			MFXInit(MFX_IMPL_HARDWARE_ANY, &ver, &auxSession);
			MFXQueryIMPL(auxSession, &impl);
			MFXClose(auxSession);
		}
		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); i++)
		{
			if (implTypes[i].impl == baseImpl)
			{
				adapters = implTypes[i].adapterID;
				break;
			}
		}
		return adapters;
	}
	INT GetMSDKAdapters(mfxSession session)
	{
		INT adapters = -1;
		mfxIMPL impl = MFX_IMPL_SOFTWARE;
		if (session)
		{
			MFXQueryIMPL(session, &impl);
		}
		else
		{
			mfxSession auxSession;
			memset(&auxSession, 0, sizeof(auxSession));

			mfxVersion ver = { 1, 1 };
			MFXInit(MFX_IMPL_HARDWARE_ANY, &ver, &auxSession);
			MFXQueryIMPL(auxSession, &impl);
			MFXClose(auxSession);
		}
		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); ++i)
		{
			if (implTypes[i].impl == baseImpl)
			{
				adapters = implTypes[i].adapterID;
				break;
			}
		}
		return adapters;
	}
	mfxU16 GetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize)
	{
		if (pSurfacesPool)
		{
			for (mfxU16 i = 0; i < nPoolSize; i++)
			{
				if (0 == pSurfacesPool[i].Data.Locked)
				{
					return i;
				}
			}
		}
		return MSDK_INVALID_SURF_IDX;
	}
}