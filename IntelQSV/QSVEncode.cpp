#include "stdafx.h"
#include "QSVEncode.h"

namespace QSV
{
	QSVEncode::QSVEncode()
	{
	}
	QSVEncode::~QSVEncode()
	{
	}
	mfxStatus QSVEncode::Open(const QSVParam& param)
	{
		mfxStatus sts = MFX_ERR_NONE;
		mfxIMPL impl = MFX_IMPL_AUTO_ANY | MFX_IMPL_VIA_D3D11;
		mfxVersion ver = { { 1, 1 } };
		sts = ::Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		memset(&m_videoParam, 0, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParam.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
		m_videoParam.mfx.TargetKbps = param.wKbps;
		m_videoParam.mfx.RateControlMethod = MFX_RATECONTROL_VBR;
		m_videoParam.mfx.FrameInfo.FrameRateExtN = param.wFPS;
		m_videoParam.mfx.FrameInfo.FrameRateExtD = 1;
		m_videoParam.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		m_videoParam.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoParam.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoParam.mfx.FrameInfo.CropX = 0;
		m_videoParam.mfx.FrameInfo.CropY = 0;
		m_videoParam.mfx.FrameInfo.CropW = param.wCX;
		m_videoParam.mfx.FrameInfo.CropH = param.wCY;
		m_videoParam.mfx.FrameInfo.Width = MSDK_ALIGN16(param.wCX);
		m_videoParam.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoParam.mfx.FrameInfo.PicStruct) ? MSDK_ALIGN16(param.wCY) : MSDK_ALIGN32(param.wCY);
		m_videoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

		memset(&m_videoVPPParam, 0, sizeof(m_videoVPPParam));
		m_videoVPPParam.vpp.In.FourCC = MFX_FOURCC_RGB4;
		m_videoVPPParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoVPPParam.vpp.In.CropX = 0;
		m_videoVPPParam.vpp.In.CropY = 0;
		m_videoVPPParam.vpp.In.CropW = param.wCX;
		m_videoVPPParam.vpp.In.CropH = param.wCY;
		m_videoVPPParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoVPPParam.vpp.In.FrameRateExtN = param.wFPS;
		m_videoVPPParam.vpp.In.FrameRateExtD = 1;
		m_videoVPPParam.vpp.In.Width = MSDK_ALIGN16(param.wCX);
		m_videoVPPParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoVPPParam.vpp.In.PicStruct) ? MSDK_ALIGN16(param.wCY) : MSDK_ALIGN32(param.wCY);
		m_videoVPPParam.vpp.Out.FourCC = MFX_FOURCC_NV12;
		m_videoVPPParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoVPPParam.vpp.Out.CropX = 0;
		m_videoVPPParam.vpp.Out.CropY = 0;
		m_videoVPPParam.vpp.Out.CropW = param.wCX;
		m_videoVPPParam.vpp.Out.CropH = param.wCY;
		m_videoVPPParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoVPPParam.vpp.Out.FrameRateExtN = param.wFPS;
		m_videoVPPParam.vpp.Out.FrameRateExtD = 1;
		m_videoVPPParam.vpp.Out.Width = MSDK_ALIGN16(m_videoVPPParam.vpp.Out.CropW);
		m_videoVPPParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoVPPParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_videoVPPParam.vpp.Out.CropH) : MSDK_ALIGN32(m_videoVPPParam.vpp.Out.CropH);
		m_videoVPPParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;

		m_videoENCODE.Reset(new MFXVideoENCODE(m_session));
		MSDK_CHECK_POINTER(m_videoENCODE, MFX_ERR_MEMORY_ALLOC);
		m_videoVPP.Reset(new MFXVideoVPP(m_session));
		MSDK_CHECK_POINTER(m_videoVPP, MFX_ERR_MEMORY_ALLOC);

		memset(&m_videoRequest, 0, sizeof(m_videoRequest));
		sts = m_videoENCODE->QueryIOSurf(&m_videoParam, &m_videoRequest);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


		mfxFrameAllocRequest videoVPPRequest[2];
		memset(&videoVPPRequest, 0, sizeof(mfxFrameAllocResponse) * 2);
		sts = m_videoVPP->QueryIOSurf(&m_videoVPPParam, videoVPPRequest);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_videoRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT;
		m_vppIN = videoVPPRequest[0].NumFrameSuggested;
		m_vppOUT = m_videoRequest.NumFrameSuggested + videoVPPRequest[1].NumFrameSuggested;
		m_videoRequest.NumFrameSuggested = m_vppOUT;
		videoVPPRequest[0].Type |= WILL_WRITE;

		mfxFrameAllocResponse	videoVPPINReponse;
		mfxFrameAllocResponse	videoVPPOUTReponse;
		memset(&videoVPPINReponse, 0, sizeof(mfxFrameAllocResponse));
		memset(&videoVPPOUTReponse, 0, sizeof(mfxFrameAllocResponse));

		sts = m_allocator.Alloc(m_allocator.pthis, &videoVPPRequest[0], &videoVPPINReponse);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		sts = m_allocator.Alloc(m_allocator.pthis, &m_videoRequest, &videoVPPOUTReponse);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		m_surfaceVPPIN.Reset(new mfxFrameSurface1*[m_vppIN]);
		MSDK_CHECK_POINTER(m_surfaceVPPIN, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_vppIN; i++)
		{
			m_surfaceVPPIN[i] = new mfxFrameSurface1;
			memset(m_surfaceVPPIN[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceVPPIN[i]->Info), &(m_videoVPPParam.vpp.In), sizeof(mfxFrameInfo));
			m_surfaceVPPIN[i]->Data.MemId = videoVPPINReponse.mids[i];
			ClearRGBSurfaceVMem(m_surfaceVPPIN[i]->Data.MemId);
		}
		m_surfaceVPPOUT.Reset(new mfxFrameSurface1*[m_vppOUT]);
		MSDK_CHECK_POINTER(m_surfaceVPPOUT, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_vppOUT; i++)
		{
			m_surfaceVPPOUT[i] = new mfxFrameSurface1;
			memset(m_surfaceVPPOUT[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceVPPOUT[i]->Info), &(m_videoVPPParam.vpp.Out), sizeof(mfxFrameInfo));
			m_surfaceVPPOUT[i]->Data.MemId = videoVPPOUTReponse.mids[i];
		}

		mfxExtVPPDoNotUse extDoNotUse;
		memset(&extDoNotUse, 0, sizeof(mfxExtVPPDoNotUse));
		extDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
		extDoNotUse.Header.BufferSz = sizeof(mfxExtVPPDoNotUse);
		extDoNotUse.NumAlg = 4;
		extDoNotUse.AlgList = new mfxU32[extDoNotUse.NumAlg];
		MSDK_CHECK_POINTER(extDoNotUse.AlgList, MFX_ERR_MEMORY_ALLOC);
		extDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		extDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		extDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		extDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;

		mfxExtBuffer* extBuffers[1];
		extBuffers[0] = (mfxExtBuffer*)& extDoNotUse;
		m_videoVPPParam.ExtParam = extBuffers;
		m_videoVPPParam.NumExtParam = 1;


		sts = m_videoENCODE->Init(&m_videoParam);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = m_videoVPP->Init(&m_videoVPPParam);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = m_videoENCODE->GetVideoParam(&m_videoParam);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = m_videoParam.mfx.BufferSizeInKB * 1000;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		return sts;
	}
	mfxStatus QSVEncode::Encode(BYTE* data, LONG size)
	{
		mfxStatus sts = MFX_ERR_NONE;
		INT indexVPPIN = 0;
		INT indexVPPOUT = 0;
		mfxSyncPoint syncpVPP;
		mfxSyncPoint syncpVideo;
		while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts)
		{
			indexVPPIN = GetFreeSurfaceIndex(m_surfaceVPPIN, m_vppIN);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPIN, MFX_ERR_MEMORY_ALLOC);
			mfxFrameSurface1* surface = m_surfaceVPPIN[indexVPPIN];
			sts = m_allocator.Lock(m_allocator.pthis, surface->Data.MemId, &(surface->Data));
			MSDK_BREAK_ON_ERROR(sts);
			mfxU16 w = 0;
			mfxU16 h = 0;
			mfxFrameInfo* pInfo = &surface->Info;
			if (pInfo->CropH > 0 && pInfo->CropW > 0)
			{
				w = pInfo->CropW;
				h = pInfo->CropH;
			}
			else
			{
				w = pInfo->Width;
				h = pInfo->Height;
			}
			ASSERT(size == h * surface->Data.Pitch);
			for (mfxU16 i = 0; i < h; i++)
			{
				memcpy(surface->Data.B + i * surface->Data.Pitch, data, w * 4);
			}
			sts = m_allocator.Unlock(m_allocator.pthis, m_surfaceVPPIN[indexVPPIN]->Data.MemId, &(m_surfaceVPPIN[indexVPPIN]->Data));
			MSDK_BREAK_ON_ERROR(sts);
			indexVPPOUT = GetFreeSurfaceIndex(m_surfaceVPPOUT, m_vppOUT);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPOUT, MFX_ERR_MEMORY_ALLOC);

			for (;;)
			{
				sts = m_videoVPP->RunFrameVPPAsync(m_surfaceVPPIN[indexVPPIN], m_surfaceVPPOUT[indexVPPOUT], NULL, &syncpVPP);
				if (MFX_WRN_DEVICE_BUSY != sts)
					break;
				MSDK_SLEEP(1);
			}
			if (MFX_ERR_MORE_DATA == sts)
				continue;
			MSDK_BREAK_ON_ERROR(sts);

			for (;;)
			{
				sts = m_videoENCODE->EncodeFrameAsync(NULL, m_surfaceVPPOUT[indexVPPOUT], &m_bitstream, &syncpVideo);
				if (MFX_ERR_NONE < sts && !syncpVideo)
				{
					if (MFX_WRN_DEVICE_BUSY == sts)
						MSDK_SLEEP(1);
				}
				else if (MFX_ERR_NONE < sts && syncpVideo)
				{
					sts = MFX_ERR_NONE;
					break;
				}
				else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
				{
					break;
				}
				else
					break;
			}
			if (MFX_ERR_NONE == sts)
			{
				sts = m_session.SyncOperation(syncpVideo, 60000);
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
				//sts = WriteBitStreamFrame(&m_bitstream, fSink);
				MSDK_BREAK_ON_ERROR(sts);
			}
		}

		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		return sts;
	}
	mfxStatus QSVEncode::Close()
	{
		return MFX_ERR_NONE;
	}
}

