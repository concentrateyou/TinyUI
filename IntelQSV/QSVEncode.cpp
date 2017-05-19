#include "stdafx.h"
#include "QSVEncode.h"
#include <VersionHelpers.h>

namespace QSV
{
	QSVEncode::QSVEncode()
		:m_syncpVPP(NULL),
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
		param.wRC = MFX_RATECONTROL_CBR;
		param.wFPS = 30;
		param.wQPB = param.wQPI = param.wQPP = 23;
		param.wConvergence = 1;
		param.wAccuracy = 1000;
		param.wMaxKbps = 3000;
		param.wKbps = wKbps;
		param.wAsyncDepth = 4;
		param.wKeyPerSec = wKbps * 2;
		param.wbFrames = 3;
		return param;
	}
	mfxStatus QSVEncode::InitializeVPPParam(const QSVParam& param)
	{
		ZeroMemory(&m_videoVPPParam, sizeof(m_videoVPPParam));
		m_videoVPPParam.vpp.In.FourCC = MFX_FOURCC_RGB4;
		m_videoVPPParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoVPPParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoVPPParam.vpp.In.CropX = 0;
		m_videoVPPParam.vpp.In.CropY = 0;
		m_videoVPPParam.vpp.In.CropW = param.wCX;
		m_videoVPPParam.vpp.In.CropH = param.wCY;
		m_videoVPPParam.vpp.In.FrameRateExtN = param.wFPS;
		m_videoVPPParam.vpp.In.FrameRateExtD = 1;
		m_videoVPPParam.vpp.In.Width = MSDK_ALIGN16(param.wCX);
		m_videoVPPParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoVPPParam.vpp.In.PicStruct) ? MSDK_ALIGN16(param.wCY) : MSDK_ALIGN32(param.wCY);
		m_videoVPPParam.vpp.Out.FourCC = MFX_FOURCC_NV12;
		m_videoVPPParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoVPPParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoVPPParam.vpp.Out.CropX = 0;
		m_videoVPPParam.vpp.Out.CropY = 0;
		m_videoVPPParam.vpp.Out.CropW = param.wCX;
		m_videoVPPParam.vpp.Out.CropH = param.wCY;
		m_videoVPPParam.vpp.Out.FrameRateExtN = param.wFPS;
		m_videoVPPParam.vpp.Out.FrameRateExtD = 1;
		m_videoVPPParam.vpp.Out.Width = MSDK_ALIGN16(m_videoVPPParam.vpp.Out.CropW);
		m_videoVPPParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoVPPParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_videoVPPParam.vpp.Out.CropH) : MSDK_ALIGN32(m_videoVPPParam.vpp.Out.CropH);
		m_videoVPPParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		m_videoVPPParam.AsyncDepth = param.wAsyncDepth;
		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncode::InitializeEncodeParam(const QSVParam& param)
	{

		ZeroMemory(&m_videoParam, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
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
		m_videoParam.AsyncDepth = param.wAsyncDepth;
		m_videoParam.mfx.GopPicSize = param.wKeyPerSec;
		m_videoParam.mfx.GopRefDist = param.wbFrames + 1;
		m_videoParam.mfx.FrameInfo.Width = MSDK_ALIGN16(param.wCX);
		m_videoParam.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_videoParam.mfx.FrameInfo.PicStruct) ? MSDK_ALIGN16(param.wCY) : MSDK_ALIGN32(param.wCY);
		m_videoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		switch (param.wRC)
		{
		case MFX_RATECONTROL_CBR://恒定码率
			m_videoParam.mfx.TargetKbps = param.wKbps;
			break;
		case MFX_RATECONTROL_VBR://可变码率
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
		}

		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncode::Allocate(const QSVParam& param)
	{
		mfxStatus status = MFX_ERR_NONE;
		m_videoENCODE.Reset(new MFXVideoENCODE(m_session));
		MSDK_CHECK_POINTER(m_videoENCODE, MFX_ERR_MEMORY_ALLOC);
		m_videoVPP.Reset(new MFXVideoVPP(m_session));
		MSDK_CHECK_POINTER(m_videoVPP, MFX_ERR_MEMORY_ALLOC);

		mfxFrameAllocRequest encodeRequest;
		ZeroMemory(&encodeRequest, sizeof(encodeRequest));
		status = m_videoENCODE->QueryIOSurf(&m_videoParam, &encodeRequest);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		if (encodeRequest.NumFrameSuggested < param.wAsyncDepth)
		{
			return MFX_ERR_MEMORY_ALLOC;
		}
		mfxU16 encodes = encodeRequest.NumFrameSuggested;
		mfxFrameAllocRequest vppRequest[2];
		ZeroMemory(&vppRequest, sizeof(mfxFrameAllocRequest) * 2);
		status = m_videoVPP->QueryIOSurf(&m_videoVPPParam, vppRequest);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		mfxU16 vpps = vppRequest[0].NumFrameSuggested;
		encodes += vppRequest[1].NumFrameSuggested;
		encodeRequest.NumFrameSuggested = encodeRequest.NumFrameMin = encodes;
		memcpy(&encodeRequest.Info, &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
		encodeRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT;
		status = m_allocator.Alloc(m_allocator.pthis, &vppRequest[0], &m_encodeReponse);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);

		vppRequest[0].NumFrameSuggested = vppRequest[0].NumFrameMin = vpps;
		memcpy(&vppRequest[0].Info, &(m_videoVPPParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
		status = m_allocator.Alloc(m_allocator.pthis, &encodeRequest, &m_vppReponse);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);

		m_surfaceEncode.Reset(new mfxFrameSurface1 *[m_encodeReponse.NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaceEncode, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_encodeReponse.NumFrameActual; i++)
		{
			m_surfaceEncode[i] = new mfxFrameSurface1;
			memset(m_surfaceEncode[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceEncode[i]->Info), &(m_videoVPPParam.vpp.In), sizeof(mfxFrameInfo));
			m_surfaceEncode[i]->Data.MemId = m_encodeReponse.mids[i];
			ClearRGBSurfaceVMem(m_surfaceEncode[i]->Data.MemId);
		}

		m_surfaceVPP.Reset(new mfxFrameSurface1 *[m_vppReponse.NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaceVPP, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_vppReponse.NumFrameActual; i++)
		{
			m_surfaceVPP[i] = new mfxFrameSurface1;
			memset(m_surfaceVPP[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceVPP[i]->Info), &(m_videoVPPParam.vpp.Out), sizeof(mfxFrameInfo));
			m_surfaceVPP[i]->Data.MemId = m_vppReponse.mids[i];
		}
		mfxExtCodingOption co;
		ZeroMemory(&co, sizeof(mfxExtCodingOption));
		co.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
		co.Header.BufferSz = sizeof(co);
		mfxExtBuffer* extEncode[1];
		extEncode[0] = (mfxExtBuffer*)&co;
		m_videoParam.ExtParam = &extEncode[0];
		m_videoParam.NumExtParam = 1;
		status = m_videoENCODE->Init(&m_videoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);

		mfxExtVPPDoNotUse vppDNU;
		ZeroMemory(&vppDNU, sizeof(mfxExtVPPDoNotUse));
		vppDNU.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
		vppDNU.Header.BufferSz = sizeof(mfxExtVPPDoNotUse);
		vppDNU.NumAlg = 4;
		mfxU32 algList[4];
		vppDNU.AlgList = algList;
		MSDK_CHECK_POINTER(vppDNU.AlgList, MFX_ERR_MEMORY_ALLOC);
		vppDNU.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		vppDNU.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		vppDNU.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		vppDNU.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;
		mfxExtBuffer* extVVP[1];
		extVVP[0] = (mfxExtBuffer *)&vppDNU;
		m_videoVPPParam.ExtParam = &extVVP[0];
		m_videoVPPParam.NumExtParam = 1;
		status = m_videoVPP->Init(&m_videoVPPParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		mfxVideoParam par;
		ZeroMemory(&par, sizeof(par));
		status = m_videoENCODE->GetVideoParam(&par);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		ZeroMemory(&m_bitstream, sizeof(m_bitstream));
		m_bitstream.MaxLength = par.mfx.BufferSizeInKB * 1000;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		return status;
	}
	mfxStatus QSVEncode::Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = InitializeEncodeParam(param);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = InitializeVPPParam(param);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = Allocate(param);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_callback = std::move(callback);
		return status;
	}
	BOOL QSVEncode::GetSPSPPS(vector<mfxU8>& sps, vector<mfxU8>& pps)
	{
		mfxU8	mfxsps[100];
		mfxU8	mfxpps[100];
		mfxVideoParam par;
		ZeroMemory(&par, sizeof(par));
		mfxExtCodingOptionSPSPPS spspps;
		ZeroMemory(&spspps, sizeof(mfxExtCodingOptionSPSPPS));
		spspps.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
		spspps.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);
		spspps.PPSBuffer = mfxpps;
		spspps.PPSBufSize = 100;
		spspps.SPSBuffer = mfxsps;
		spspps.SPSBufSize = 100;
		mfxExtBuffer* ext[1];
		ext[0] = (mfxExtBuffer *)&spspps;
		par.ExtParam = &ext[0];
		par.NumExtParam = 1;
		if (m_videoENCODE->GetVideoParam(&par) == MFX_ERR_NONE)
		{
			sps.resize(spspps.SPSBufSize);
			memcpy(&sps[0], mfxsps, spspps.SPSBufSize);
			pps.resize(spspps.PPSBufSize);
			memcpy(&sps[0], mfxpps, spspps.PPSBufSize);
			return TRUE;
		}
		return FALSE;
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
			mfxU16 index = h - i - 1;
			memcpy(surface->Data.B + i * surface->Data.Pitch, data + index * w * 4, w * 4);
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
		INT indexVPPIN = GetFreeSurfaceIndex(m_surfaceEncode, m_encodeReponse.NumFrameActual);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPIN, MFX_ERR_MEMORY_ALLOC);
		INT indexVPPOUT = GetFreeSurfaceIndex(m_surfaceVPP, m_vppReponse.NumFrameActual);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexVPPOUT, MFX_ERR_MEMORY_ALLOC);
		status = m_allocator.Lock(m_allocator.pthis, m_surfaceEncode[indexVPPIN]->Data.MemId, &(m_surfaceEncode[indexVPPIN]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		this->LoadRGBA(m_surfaceEncode[indexVPPIN], data, size);
		status = m_allocator.Unlock(m_allocator.pthis, m_surfaceEncode[indexVPPIN]->Data.MemId, &(m_surfaceEncode[indexVPPIN]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = EncodeVPP(m_surfaceEncode[indexVPPIN], m_surfaceVPP[indexVPPOUT]);
		if (MFX_ERR_MORE_DATA == status)
			return status;
		status = EncodeVideo(m_surfaceVPP[indexVPPOUT]);
		if (MFX_ERR_MORE_DATA == status)
			return status;
		if (MFX_ERR_NONE == status)
		{
			status = m_session.SyncOperation(m_syncpVideo, MSDK_WAIT_INTERVAL);
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			if (!m_callback.IsNull())
			{
				m_callback(m_bitstream.Data + m_bitstream.DataOffset, m_bitstream.DataLength);
				/*if (m_bitstream.FrameType & MFX_FRAMETYPE_I)
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
				}*/
			}
			m_bitstream.DataLength = 0;
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
		for (mfxU16 i = 0; i < m_encodeReponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_surfaceEncode[i]);
		}
		m_surfaceEncode.Reset(NULL);
		for (mfxU16 i = 0; i < m_vppReponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_surfaceVPP[i]);
		}
		m_surfaceVPP.Reset(NULL);
		SAFE_DELETE_ARRAY(m_bitstream.Data);
		m_allocator.Free(m_allocator.pthis, &m_encodeReponse);
		m_allocator.Free(m_allocator.pthis, &m_vppReponse);
		Release();
		mfxStatus sts = MFX_ERR_NONE;
		sts = m_session.Close();
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		return sts;
	}
}

