#include "stdafx.h"
#include "QSVEncode.h"
#include <VersionHelpers.h>

namespace QSV
{
	QSVColorSpaceConverter::QSVColorSpaceConverter()
	{

	}
	QSVColorSpaceConverter::~QSVColorSpaceConverter()
	{

	}
	mfxStatus QSVColorSpaceConverter::Initialize(const TinySize& i, mfxU32 iFourCC, const TinySize& o, mfxU32 oFourCC)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_vppParams, 0, sizeof(m_vppParams));
		m_vppParams.vpp.In.FourCC = iFourCC;
		m_vppParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParams.vpp.In.CropX = 0;
		m_vppParams.vpp.In.CropY = 0;
		m_vppParams.vpp.In.CropW = i.cx;
		m_vppParams.vpp.In.CropH = i.cy;
		m_vppParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;//ÖðÐÐÉ¨ÃèÍ¼Ïñ
		m_vppParams.vpp.In.FrameRateExtN = 30;
		m_vppParams.vpp.In.FrameRateExtD = 1;
		m_vppParams.vpp.In.Width = MSDK_ALIGN16(cx);
		m_vppParams.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParams.vpp.In.PicStruct) ? MSDK_ALIGN16(i.cx) : MSDK_ALIGN32(i.cy);
		m_vppParams.vpp.Out.FourCC = oFourCC;
		m_vppParams.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParams.vpp.Out.CropX = 0;
		m_vppParams.vpp.Out.CropY = 0;
		m_vppParams.vpp.Out.CropW = o.cx;
		m_vppParams.vpp.Out.CropH = o.cy;
		m_vppParams.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_vppParams.vpp.Out.FrameRateExtN = 30;
		m_vppParams.vpp.Out.FrameRateExtD = 1;
		m_vppParams.vpp.Out.Width = MSDK_ALIGN16(m_vppParams.vpp.Out.CropW);
		m_vppParams.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParams.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_vppParams.vpp.Out.CropH) : MSDK_ALIGN32(m_vppParams.vpp.Out.CropH);
		m_vppParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		m_mfxVPP.Reset(new MFXVideoVPP(m_session));

		mfxFrameAllocRequest vvpRequest[2];
		memset(&vvpRequest, 0, sizeof(mfxFrameAllocRequest) * 2);
		status = m_mfxVPP.QueryIOSurf(&m_vppParams, vvpRequest);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		vvpRequest[0].Type |= WILL_WRITE;
		vvpRequest[1].Type |= WILL_READ;
		mfxFrameAllocResponse mfxResponseI;
		mfxFrameAllocResponse mfxResponseO;
		status = m_mfxAllocator.Alloc(m_mfxAllocator.pthis, &vvpRequest[0], &mfxResponseI);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, status);
		status = m_mfxAllocator.Alloc(m_mfxAllocator.pthis, &vvpRequest[1], &mfxResponseO);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_surfacesI = mfxResponseI.NumFrameActual;
		m_surfacesO = mfxResponseO.NumFrameActual;
		m_pvvpSurfacesI = new mfxFrameSurface1 *[sI];
		MSDK_CHECK_POINTER(m_pvvpSurfacesI, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < sI; i++)
		{
			m_pvvpSurfacesI[i] = new mfxFrameSurface1();
			memset(m_pvvpSurfacesI[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_pvvpSurfacesI[i]->Info), &(m_vppParams.vpp.In), sizeof(mfxFrameInfo));
			m_pvvpSurfacesI[i]->Data.MemId = mfxResponseI.mids[i];
		}

		m_pvvpSurfacesO = new mfxFrameSurface1 *[sO];
		MSDK_CHECK_POINTER(m_pvvpSurfacesO, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < sO; i++)
		{
			m_pvvpSurfacesO[i] = new mfxFrameSurface1();
			memset(m_pvvpSurfacesO[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_pvvpSurfacesO[i]->Info), &(m_vppParams.vpp.Out), sizeof(mfxFrameInfo));
			m_pvvpSurfacesO[i]->Data.MemId = mfxResponseO.mids[i];
		}

		mfxExtVPPDoUse extDoUse;
		memset(&extDoUse, 0, sizeof(extDoUse));
		mfxU32 tabDoUseAlg[1];
		extDoUse.Header.BufferId = MFX_EXTBUFF_VPP_DOUSE;
		extDoUse.Header.BufferSz = sizeof(mfxExtVPPDoUse);
		extDoUse.NumAlg = 1;
		extDoUse.AlgList = tabDoUseAlg;
		tabDoUseAlg[0] = MFX_EXTBUFF_VPP_DENOISE;
		mfxExtVPPDenoise denoiseConfig;
		memset(&denoiseConfig, 0, sizeof(denoiseConfig));
		denoiseConfig.Header.BufferId = MFX_EXTBUFF_VPP_DENOISE;
		denoiseConfig.Header.BufferSz = sizeof(mfxExtVPPDenoise);
		denoiseConfig.DenoiseFactor = 100;
		mfxExtBuffer* extBuffer[2];
		extBuffer[0] = (mfxExtBuffer*)&extDoUse;
		extBuffer[1] = (mfxExtBuffer*)&denoiseConfig;
		m_vppParams.NumExtParam = 2;
		m_vppParams.ExtParam = (mfxExtBuffer**)&extBuffer[0];
		status = m_mfxVPP.Init(&m_vppParams);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	mfxStatus QSVColorSpaceConverter::Convert(Callback<void(mfxFrameSurface1&,BYTE*)>&& callbackI)
	{
		mfxStatus status = MFX_ERR_NONE;
		UINT surfaceIndexI = 0;
		UINT surfaceIndexO = 0;
		while (MFX_ERR_NONE <= status || MFX_ERR_MORE_DATA == status)
		{
			surfaceIndexI = GetFreeSurfaceIndex(m_pvvpSurfacesI, m_surfacesI);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, surfaceIndexI, MFX_ERR_MEMORY_ALLOC);
			mfxFrameSurface1* pvvpSurfaceI = m_pvvpSurfacesI[surfaceIndexI];
			status = m_mfxAllocator.Lock(m_mfxAllocator.pthis, pvvpSurfaceI->Data.MemId, &(pvvpSurfaceI->Data));
			MSDK_BREAK_ON_ERROR(status);
			mfxFrameData* ps = pvvpSurfaceI->Data;

		}
	}
}

