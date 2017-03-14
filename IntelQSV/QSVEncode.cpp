#include "stdafx.h"
#include "QSVEncode.h"
#include <VersionHelpers.h>

namespace QSV
{
	QSVEncode::QSVEncode()
		:m_vppIN(0),
		m_vppOUT(0),
		m_syncpVPP(NULL),
		m_syncpVideo(NULL)
	{

	}
	QSVEncode::~QSVEncode()
	{
	}
	QSVParam QSVEncode::GetDefaultQSV(WORD wCX, WORD wCY, WORD wKbps)
	{
		QSVParam param = { 0 };
		param.wTargetUsage = MFX_TARGETUSAGE_BALANCED;
		param.wCX = wCX;
		param.wCY = wCY;
		param.wRC = MFX_RATECONTROL_VBR;
		param.wFPS = 30;
		param.wQPB = param.wQPI = param.wQPP = 23;
		param.wConvergence = 1;
		param.wAccuracy = 1000;
		param.wMaxKbps = 3000;
		param.wKbps = wKbps;
		param.wAsyncDepth = 4;
		param.wICQQuality = 23;
		param.wKeyPerSec = 3;
		param.wbFrames = 7;
		param.wLookAheadDepth = 40;
		return param;
	}
	mfxStatus QSVEncode::Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		ZeroMemory(&m_videoParam, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParam.mfx.GopOptFlag = MFX_GOP_STRICT;
		m_videoParam.mfx.NumSlice = 1;
		m_videoParam.mfx.CodecProfile = MFX_PROFILE_AVC_BASELINE;
		m_videoParam.mfx.TargetUsage = param.wTargetUsage;
		m_videoParam.mfx.FrameInfo.FrameRateExtN = param.wFPS;
		m_videoParam.mfx.FrameInfo.FrameRateExtD = 1;
		m_videoParam.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		m_videoParam.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoParam.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoParam.mfx.FrameInfo.CropX = 0;
		m_videoParam.mfx.FrameInfo.CropY = 0;
		m_videoParam.mfx.FrameInfo.CropW = param.wCX;
		m_videoParam.mfx.FrameInfo.CropH = param.wCY;
		m_videoParam.mfx.RateControlMethod = param.wRC;
		switch (param.wRC)
		{
		case MFX_RATECONTROL_LA:
		case MFX_RATECONTROL_CBR://恒定码率
			m_videoParam.mfx.TargetKbps = param.wKbps;
			break;
		case MFX_RATECONTROL_VBR:
		case MFX_RATECONTROL_VCM://
			m_videoParam.mfx.TargetKbps = param.wKbps;
			m_videoParam.mfx.MaxKbps = param.wMaxKbps;
			break;
		case MFX_RATECONTROL_CQP://恒定质量
			m_videoParam.mfx.QPI = param.wQPI;
			m_videoParam.mfx.QPB = param.wQPB;
			m_videoParam.mfx.QPP = param.wQPP;
			break;
		case MFX_RATECONTROL_AVBR://平均码率
			m_videoParam.mfx.TargetKbps = param.wKbps;
			m_videoParam.mfx.Accuracy = param.wAccuracy;
			m_videoParam.mfx.Convergence = param.wConvergence;
			break;
		case MFX_RATECONTROL_ICQ:
		case MFX_RATECONTROL_LA_ICQ:
			m_videoParam.mfx.ICQQuality = param.wICQQuality;
			break;
		default:
			break;
		}
		m_videoParam.AsyncDepth = param.wAsyncDepth;
		m_videoParam.mfx.GopPicSize = (mfxU16)(param.wKeyPerSec *param.wFPS);
		static mfxExtBuffer* extendedBuffers[2];
		INT iBuffers = 0;
		if (param.wAsyncDepth == 1)
		{
			m_videoParam.mfx.NumRefFrame = 1;
			m_videoParam.mfx.GopRefDist = 1;
			memset(&m_co, 0, sizeof(mfxExtCodingOption));
			m_co.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
			m_co.Header.BufferSz = sizeof(mfxExtCodingOption);
			m_co.MaxDecFrameBuffering = 1;
			extendedBuffers[iBuffers++] = (mfxExtBuffer*)&m_co;
		}
		else
		{
			m_videoParam.mfx.GopRefDist = param.wbFrames + 1;
		}
		if (param.wRC == MFX_RATECONTROL_LA_ICQ || param.wRC == MFX_RATECONTROL_LA)
		{
			memset(&m_co2, 0, sizeof(mfxExtCodingOption2));
			m_co2.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
			m_co2.Header.BufferSz = sizeof(m_co2);
			m_co2.LookAheadDepth = param.wLookAheadDepth;
			extendedBuffers[iBuffers++] = (mfxExtBuffer*)& m_co2;
		}
		if (iBuffers > 0)
		{
			m_videoParam.ExtParam = extendedBuffers;
			m_videoParam.NumExtParam = (mfxU16)iBuffers;
		}
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
		mfxFrameAllocRequest videoRequest;
		memset(&videoRequest, 0, sizeof(videoRequest));
		status = m_videoENCODE->Query(&m_videoParam, &m_videoParam);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = m_videoENCODE->QueryIOSurf(&m_videoParam, &videoRequest);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		mfxFrameAllocRequest videoVPPRequest[2];
		memset(&videoVPPRequest, 0, sizeof(mfxFrameAllocRequest) * 2);
		status = m_videoVPP->QueryIOSurf(&m_videoVPPParam, videoVPPRequest);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		videoRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT;
		m_vppIN = videoVPPRequest[0].NumFrameSuggested;
		m_vppOUT = videoRequest.NumFrameSuggested + videoVPPRequest[1].NumFrameSuggested;
		videoRequest.NumFrameSuggested = m_vppOUT;
		videoVPPRequest[0].Type |= WILL_WRITE;
		status = m_allocator.Alloc(m_allocator.pthis, &videoVPPRequest[0], &m_videoVPPINReponse);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = m_allocator.Alloc(m_allocator.pthis, &videoRequest, &m_videoVPPOUTReponse);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_surfaceVPPIN.Reset(new mfxFrameSurface1 *[m_vppIN]);
		MSDK_CHECK_POINTER(m_surfaceVPPIN, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_vppIN; i++)
		{
			m_surfaceVPPIN[i] = new mfxFrameSurface1;
			memset(m_surfaceVPPIN[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceVPPIN[i]->Info), &(m_videoVPPParam.vpp.In), sizeof(mfxFrameInfo));
			m_surfaceVPPIN[i]->Data.MemId = m_videoVPPINReponse.mids[i];
			ClearRGBSurfaceVMem(m_surfaceVPPIN[i]->Data.MemId);
		}

		m_surfaceVPPOUT.Reset(new mfxFrameSurface1 *[m_vppOUT]);
		MSDK_CHECK_POINTER(m_surfaceVPPOUT, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_vppOUT; i++)
		{
			m_surfaceVPPOUT[i] = new mfxFrameSurface1;
			memset(m_surfaceVPPOUT[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceVPPOUT[i]->Info), &(m_videoVPPParam.vpp.Out), sizeof(mfxFrameInfo));
			m_surfaceVPPOUT[i]->Data.MemId = m_videoVPPOUTReponse.mids[i];
		}

		memset(&m_vppDoNotUse, 0, sizeof(mfxExtVPPDoNotUse));
		m_vppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
		m_vppDoNotUse.Header.BufferSz = sizeof(mfxExtVPPDoNotUse);
		m_vppDoNotUse.NumAlg = 4;
		m_vppDoNotUse.AlgList = new mfxU32[m_vppDoNotUse.NumAlg];
		MSDK_CHECK_POINTER(m_vppDoNotUse.AlgList, MFX_ERR_MEMORY_ALLOC);
		m_vppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		m_vppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		m_vppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		m_vppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;
		mfxExtBuffer* vppBuffers[1];
		vppBuffers[0] = (mfxExtBuffer*)& m_vppDoNotUse;
		m_videoVPPParam.ExtParam = vppBuffers;
		m_videoVPPParam.NumExtParam = 1;
		status = m_videoENCODE->Init(&m_videoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = m_videoVPP->Init(&m_videoVPPParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		mfxVideoParam par;
		memset(&par, 0, sizeof(par));
		status = m_videoENCODE->GetVideoParam(&par);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = par.mfx.BufferSizeInKB * 1000;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		m_callback = std::move(callback);
		return status;
	}
	void QSVEncode::LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size)
	{
		mfxU16 w, h;
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
		ASSERT(size == surface->Data.Pitch * h);
		for (mfxU16 i = 0; i < h; i++)
		{
			memcpy(surface->Data.B + i * surface->Data.Pitch, data + i * surface->Data.Pitch, w * 4);
		}
	}
	mfxStatus QSVEncode::EncodeVPP(mfxFrameSurface1* surfaceIN, mfxFrameSurface1* surfaceOUT)
	{
		mfxStatus sts = MFX_ERR_NONE;
		for (;;)
		{
			sts = m_videoVPP->RunFrameVPPAsync(surfaceIN, surfaceOUT, NULL, &m_syncpVPP);
			if (MFX_WRN_DEVICE_BUSY == sts)
			{
				MSDK_SLEEP(1);
				continue;
			}
			break;
		}
		return sts;
	}
	mfxStatus QSVEncode::EncodeVideo(mfxFrameSurface1* surfaceOUT)
	{
		mfxStatus status = MFX_ERR_NONE;
		for (;;)
		{
			m_bitstream.TimeStamp = timeGetTime();
			status = m_videoENCODE->EncodeFrameAsync(NULL, surfaceOUT, &m_bitstream, &m_syncpVideo);
			if (MFX_ERR_NONE < status && !m_syncpVideo)
			{
				if (MFX_WRN_DEVICE_BUSY == status)
					MSDK_SLEEP(1);
				continue;
			}
			if (MFX_ERR_NONE < status && m_syncpVideo)
			{
				status = MFX_ERR_NONE;
				break;
			}
			break;
		}
		return status;
	}
	mfxStatus QSVEncode::Encode(BYTE* data, LONG size)
	{
		mfxStatus status = MFX_ERR_NONE;
		INT indexVPPIN = GetFreeSurfaceIndex(m_surfaceVPPIN, m_vppIN);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPIN, MFX_ERR_MEMORY_ALLOC);
		INT indexVPPOUT = GetFreeSurfaceIndex(m_surfaceVPPOUT, m_vppOUT);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPOUT, MFX_ERR_MEMORY_ALLOC);
		status = m_allocator.Lock(m_allocator.pthis, m_surfaceVPPIN[indexVPPIN]->Data.MemId, &(m_surfaceVPPIN[indexVPPIN]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		this->LoadRGBA(m_surfaceVPPIN[indexVPPIN], data, size);
		status = m_allocator.Unlock(m_allocator.pthis, m_surfaceVPPIN[indexVPPIN]->Data.MemId, &(m_surfaceVPPIN[indexVPPIN]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = EncodeVPP(m_surfaceVPPIN[indexVPPIN], m_surfaceVPPOUT[indexVPPOUT]);
		if (MFX_ERR_MORE_DATA == status)
			return status;
		status = EncodeVideo(m_surfaceVPPOUT[indexVPPOUT]);
		if (MFX_ERR_MORE_DATA == status)
			return status;
		if (MFX_ERR_NONE == status)
		{
			status = m_session.SyncOperation(m_syncpVideo, 60000);
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			if (!m_callback.IsNull())
			{
				m_callback(m_bitstream.Data + m_bitstream.DataOffset, m_bitstream.DataLength);
				m_bitstream.DataLength = 0;
				if (m_bitstream.FrameType & MFX_FRAMETYPE_I)
				{
					TRACE("MFX_FRAMETYPE_I\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_P)
				{
					TRACE("MFX_FRAMETYPE_P\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_B)
				{
					TRACE("MFX_FRAMETYPE_B\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_S)
				{
					TRACE("MFX_FRAMETYPE_S\n");
				}

				if (m_bitstream.FrameType & MFX_FRAMETYPE_REF)
				{
					TRACE("MFX_FRAMETYPE_REF\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_IDR)
				{
					TRACE("MFX_FRAMETYPE_IDR\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xI)
				{
					TRACE("MFX_FRAMETYPE_xI\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xP)
				{
					TRACE("MFX_FRAMETYPE_xP\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xB)
				{
					TRACE("MFX_FRAMETYPE_xB\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xS)
				{
					TRACE("MFX_FRAMETYPE_xS\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xREF)
				{
					TRACE("MFX_FRAMETYPE_xREF\n");
				}
				if (m_bitstream.FrameType & MFX_FRAMETYPE_xIDR)
				{
					TRACE("MFX_FRAMETYPE_xIDR\n");
				}
			}
		}
		return status;
	}
	mfxStatus QSVEncode::Close()
	{
		if (m_videoENCODE != NULL)
		{
			m_videoENCODE->Close();
			m_videoENCODE.Reset(NULL);
		}
		if (m_videoVPP != NULL)
		{
			m_videoVPP->Close();
			m_videoVPP.Reset(NULL);
		}
		for (mfxU16 i = 0; i < m_vppIN; i++)
		{
			SAFE_DELETE(m_surfaceVPPIN[i]);
		}
		m_surfaceVPPIN.Reset(NULL);
		for (mfxU16 i = 0; i < m_vppOUT; i++)
		{
			SAFE_DELETE(m_surfaceVPPOUT[i]);
		}
		m_surfaceVPPOUT.Reset(NULL);
		SAFE_DELETE_ARRAY(m_bitstream.Data);
		SAFE_DELETE_ARRAY(m_vppDoNotUse.AlgList);
		m_allocator.Free(m_allocator.pthis, &m_videoVPPINReponse);
		m_allocator.Free(m_allocator.pthis, &m_videoVPPOUTReponse);
		Release();
		mfxStatus sts = MFX_ERR_NONE;
		sts = m_session.Close();
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		return sts;
	}
}

