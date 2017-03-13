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
	QSVParam QSVEncode::GetDefaultQSV(WORD wCX, WORD wCY)
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
		param.wAsyncDepth = 4;
		param.wICQQuality = 23;
		param.wKeyPerSec = 3;
		param.wbFrames = 7;
		param.wLookAheadDepth = 40;
		return param;
	}
	BOOL QSVEncode::Open(const QSVParam& param)
	{
		if (!IntelExists())
			return FALSE;
		mfxStatus sts = MFX_ERR_NONE;
		mfxIMPL impl = MFX_IMPL_AUTO_ANY | MFX_IMPL_VIA_D3D11;
		mfxVersion ver = { { 1, 1 } };
		sts = ::Initialize(impl, ver, &m_session, &m_allocator);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		m_videoENCODE.Reset(new MFXVideoENCODE(m_session));
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
		m_videoParam.mfx.FrameInfo.Height = MSDK_ALIGN16(param.wCY);
		m_videoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		sts = m_videoENCODE->Query(&m_videoParam, &m_videoParam);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		if (!Allocate())
			return FALSE;
		sts = m_videoENCODE->Init(&m_videoParam);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		sts = GetVideoParam();
		if (sts != MFX_ERR_NONE)
			return FALSE;
		m_tasks.Reset(new Task[m_parameter.AsyncDepth]);
		memset(m_tasks, 0, sizeof(Task) * m_parameter.AsyncDepth);
		for (INT i = 0; i < m_parameter.AsyncDepth; i++)
		{
			m_tasks[i].mfxBS.MaxLength = m_parameter.mfx.BufferSizeInKB * 1000;
			m_tasks[i].mfxBS.Data = new mfxU8[m_tasks[i].mfxBS.MaxLength];
			MSDK_CHECK_POINTER(m_tasks[i].mfxBS.Data, MFX_ERR_MEMORY_ALLOC);
		}
		memset(&m_bitstream, 0, sizeof(mfxBitstream));
		m_bitstream.MaxLength = m_parameter.mfx.BufferSizeInKB * 1000;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		return TRUE;
	}
	mfxStatus  QSVEncode::LoadNV12(mfxFrameSurface1 *pSurface, BYTE *pDataY, BYTE* pDataUV, UINT32 strideY, UINT32 strideUV)
	{
		mfxU16 w, h, i, pitch;
		mfxU8* ptr;
		mfxFrameInfo* pInfo = &pSurface->Info;
		mfxFrameData* pData = &pSurface->Data;
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
		pitch = pData->Pitch;
		ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;
		for (i = 0; i < h; i++)
		{
			memcpy(ptr + i * pitch, pDataY + i * strideY, w);
		}
		h /= 2;
		ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;
		for (i = 0; i < h; i++)
		{
			memcpy(ptr + i * pitch, pDataUV + i * strideUV, w);
		}
		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncode::Encode(UINT64 ts, BYTE *pDataY, BYTE *pDataUV, UINT32 strideY, UINT32 strideUV, mfxBitstream **pBS)
	{
		mfxStatus sts = MFX_ERR_NONE;
		INT surfaceIndex = GetFreeSurfaceIndex(m_surfaces, m_response.NumFrameActual);
		INT taskIndex = GetFreeTaskIndex(m_tasks, m_parameter.AsyncDepth);
		INT firstSyncTask = 0;
		*pBS = NULL;
		while (MFX_ERR_NOT_FOUND == taskIndex || MFX_ERR_NOT_FOUND == surfaceIndex)
		{
			sts = m_session.SyncOperation(m_tasks[firstSyncTask].syncp, 60000);
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
			mfxU8 *ps = m_bitstream.Data;
			memcpy(&m_bitstream, &m_tasks[firstSyncTask].mfxBS, sizeof(mfxBitstream));
			m_tasks[firstSyncTask].mfxBS.Data = ps;
			m_tasks[firstSyncTask].mfxBS.DataLength = 0;
			m_tasks[firstSyncTask].mfxBS.DataOffset = 0;
			m_tasks[firstSyncTask].syncp = NULL;
			taskIndex = firstSyncTask;
			firstSyncTask = (firstSyncTask + 1) % m_parameter.AsyncDepth;
			*pBS = &m_bitstream;
			surfaceIndex = GetFreeSurfaceIndex(m_surfaces, m_response.NumFrameActual);
		}
		mfxFrameSurface1 *pSurface = m_surfaces[surfaceIndex];
		sts = m_allocator.Lock(m_allocator.pthis, pSurface->Data.MemId, &(pSurface->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		sts = LoadNV12(pSurface, pDataY, pDataUV, strideY, strideUV);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		sts = m_allocator.Unlock(m_allocator.pthis, pSurface->Data.MemId, &(pSurface->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		for (;;)
		{
			sts = m_videoENCODE->EncodeFrameAsync(NULL, pSurface, &m_tasks[taskIndex].mfxBS, &m_tasks[taskIndex].syncp);
			if (MFX_ERR_NONE < sts && !m_tasks[taskIndex].syncp)
			{
				if (MFX_WRN_DEVICE_BUSY == sts)
					MSDK_SLEEP(1);
			}
			else if (MFX_ERR_NONE < sts && m_tasks[taskIndex].syncp)
			{
				sts = MFX_ERR_NONE;
				break;
			}
			else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
			{
				break;
			}
			else
			{
				break;
			}
		}
		return sts;
	}
	mfxStatus QSVEncode::GetVideoParam()
	{
		ASSERT(m_videoENCODE);
		memset(&m_parameter, 0, sizeof(m_parameter));
		mfxExtCodingOptionSPSPPS opt;
		memset(&m_parameter, 0, sizeof(m_parameter));
		opt.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
		opt.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);
		static mfxExtBuffer* extendedBuffers[1];
		extendedBuffers[0] = (mfxExtBuffer*)& opt;
		m_parameter.ExtParam = extendedBuffers;
		m_parameter.NumExtParam = 1;
		opt.SPSBuffer = m_sps;
		opt.PPSBuffer = m_pps;
		opt.SPSBufSize = 100;
		opt.PPSBufSize = 100;
		mfxStatus sts = m_videoENCODE->GetVideoParam(&m_parameter);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_spsSize = opt.SPSBufSize;
		m_ppsSize = opt.PPSBufSize;
		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncode::Allocate()
	{
		ASSERT(m_videoENCODE);
		mfxFrameAllocRequest request;
		memset(&request, 0, sizeof(request));
		mfxStatus sts = m_videoENCODE->QueryIOSurf(&m_videoParam, &request);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		request.Type |= WILL_WRITE;
		request.NumFrameSuggested += m_videoParam.AsyncDepth;
		sts = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_surfaces.Reset(new mfxFrameSurface1*[m_response.NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaces, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_response.NumFrameActual; i++)
		{
			m_surfaces[i] = new mfxFrameSurface1();
			memset(reinterpret_cast<void*>(m_surfaces[i]), 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaces[i]->Info), &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_surfaces[i]->Data.MemId = m_response.mids[i];
		}
		return sts;
	}
	BOOL QSVEncode::IntelExists()
	{
		TinyComPtr<IDXGIFactory1> factory;
		if (!SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
			return FALSE;
		UINT i = 0;
		TinyComPtr<IDXGIAdapter1> adapter;
		while (factory->EnumAdapters1(i++, &adapter) == S_OK)
		{
			DXGI_ADAPTER_DESC desc;
			if (!SUCCEEDED(adapter->GetDesc(&desc)))
				continue;
			if (desc.VendorId == 0x8086)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL QSVEncode::Close()
	{
		for (WORD i = 0;i < m_response.NumFrameActual;i++)
			SAFE_DELETE(m_surfaces[i]);
		m_surfaces.Reset(NULL);
		for (WORD i = 0;i < m_parameter.AsyncDepth;i++)
			SAFE_DELETE_ARRAY(m_tasks[i].mfxBS.Data);
		m_tasks.Reset(NULL);
		SAFE_DELETE_ARRAY(m_bitstream.Data);
		if (m_videoENCODE)
			return m_videoENCODE->Close() == MFX_ERR_NONE;
		return FALSE;
	}
}

