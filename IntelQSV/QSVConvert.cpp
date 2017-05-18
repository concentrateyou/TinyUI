#include "stdafx.h"
#include "QSVConvert.h"
#include "Common/TinyCollection.h"

namespace QSV
{
	QSVConvert::QSVConvert()
	{
	}

	QSVConvert::~QSVConvert()
	{
	}
	QSVParam QSVConvert::GetDefaultQSV(WORD wCX, WORD wCY, WORD wKbps)
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
	mfxStatus QSVConvert::Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = Initialize(impl, ver, &m_session, &m_allocator);
		status = InitializeVPPParam(param);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = Allocate(param);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_callback = std::move(callback);
		return status;
	}
	BOOL WriteSection(mfxU8* plane, mfxU16 factor, mfxU16 chunksize, mfxFrameInfo* pInfo, mfxFrameData* pData, mfxU32 i, mfxU32 j, TinyBufferArray<BYTE>& bits)
	{
		return bits.Add(plane + (pInfo->CropY * pData->Pitch / factor + pInfo->CropX) + i * pData->Pitch + j, chunksize);
	}

	mfxStatus QSVConvert::Convert(BYTE* data, LONG size)
	{
		mfxStatus status = MFX_ERR_NONE;
		INT indexI = GetFreeSurfaceIndex(m_surfaceI, m_reponses[0].NumFrameActual);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexI, MFX_ERR_MEMORY_ALLOC);
		status = m_allocator.Lock(m_allocator.pthis, m_surfaceI[indexI]->Data.MemId, &(m_surfaceI[indexI]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		LoadRGBA(m_surfaceI[indexI], data, size);
		status = m_allocator.Unlock(m_allocator.pthis, m_surfaceI[indexI]->Data.MemId, &(m_surfaceI[indexI]->Data));
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		INT indexO = GetFreeSurfaceIndex(m_surfaceO, m_reponses[1].NumFrameActual);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, indexO, MFX_ERR_MEMORY_ALLOC);
		for (;;)
		{
			status = m_videoVPP->RunFrameVPPAsync(m_surfaceI[indexI], m_surfaceO[indexO], NULL, &m_syncpVPP);
			if (MFX_WRN_DEVICE_BUSY == status)
			{
				MSDK_SLEEP(1);
			}
			else
				break;
		}
		if (MFX_ERR_MORE_DATA == status)
			return status;
		if (MFX_ERR_NONE == status)
		{
			status = m_session.SyncOperation(m_syncpVPP, 60000);
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			status = m_allocator.Lock(m_allocator.pthis, m_surfaceO[indexO]->Data.MemId, &(m_surfaceO[indexO]->Data));
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			if (!m_callback.IsNull())
			{
				mfxFrameInfo* pInfo = &m_surfaceO[indexO]->Info;
				mfxFrameData* pData = &m_surfaceO[indexO]->Data;
				if (!m_bits)
				{
					m_bits.Reset(new BYTE[pInfo->CropW * pInfo->CropH * 1.5]);
				}
				mfxU32 i, j, h, w;
				TinyBufferArray<BYTE> buffer;
				for (i = 0; i < pInfo->CropH; i++)
				{
					WriteSection(pData->Y, 1, pInfo->CropW, pInfo, pData, i, 0, buffer);
				}
				h = pInfo->CropH / 2;
				w = pInfo->CropW;
				for (i = 0; i < h; i++)
				{
					for (j = 0; j < w; j += 2)
					{
						WriteSection(pData->UV, 2, 1, pInfo, pData, i, j, buffer);
					}
				}
				for (i = 0; i < h; i++)
				{
					for (j = 1; j < w; j += 2)
					{
						WriteSection(pData->UV, 2, 1, pInfo, pData, i, j, buffer);
					}
				}
				m_callback(buffer.GetPointer(), buffer.GetSize());
			}
			status = m_allocator.Unlock(m_allocator.pthis, m_surfaceO[indexO]->Data.MemId, &(m_surfaceO[indexO]->Data));
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		}
		return status;
	}
	void QSVConvert::LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size)
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
			memcpy(surface->Data.B + i * surface->Data.Pitch, data + index * surface->Data.Pitch, w * 4);
		}
	}
	mfxStatus QSVConvert::Close()
	{
		if (m_videoVPP != NULL)
		{
			m_videoVPP->Close();
			m_videoVPP.Reset(NULL);
		}
		for (mfxU16 i = 0; i < m_reponses[0].NumFrameActual; i++)
		{
			SAFE_DELETE(m_surfaceI[i]);
		}
		m_surfaceI.Reset(NULL);
		for (mfxU16 i = 0; i < m_reponses[1].NumFrameActual; i++)
		{
			SAFE_DELETE(m_surfaceO[i]);
		}
		m_surfaceO.Reset(NULL);
		m_allocator.Free(m_allocator.pthis, &m_reponses[0]);
		m_allocator.Free(m_allocator.pthis, &m_reponses[1]);
		Release();
		mfxStatus sts = MFX_ERR_NONE;
		sts = m_session.Close();
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		return sts;
	}
	mfxStatus QSVConvert::InitializeVPPParam(const QSVParam& param)
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
	mfxStatus QSVConvert::Allocate(const QSVParam& param)
	{
		m_videoVPP.Reset(new MFXVideoVPP(m_session));
		mfxFrameAllocRequest requests[2];
		memset(&requests, 0, sizeof(mfxFrameAllocRequest) * 2);
		mfxStatus status = m_videoVPP->QueryIOSurf(&m_videoVPPParam, requests);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		requests[0].Type |= WILL_WRITE;
		requests[1].Type |= WILL_READ;
		mfxFrameAllocResponse mfxResponseIn;
		mfxFrameAllocResponse mfxResponseOut;
		status = m_allocator.Alloc(m_allocator.pthis, &requests[0], &m_reponses[0]);//IN
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = m_allocator.Alloc(m_allocator.pthis, &requests[1], &m_reponses[1]);//OUT
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_surfaceI.Reset(new mfxFrameSurface1 *[m_reponses[0].NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaceI, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_reponses[0].NumFrameActual; i++)
		{
			m_surfaceI[i] = new mfxFrameSurface1;
			memset(m_surfaceI[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceI[i]->Info), &(m_videoVPPParam.vpp.In), sizeof(mfxFrameInfo));
			m_surfaceI[i]->Data.MemId = m_reponses[0].mids[i];
		}
		m_surfaceO.Reset(new mfxFrameSurface1 *[m_reponses[1].NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaceO, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_reponses[1].NumFrameActual; i++)
		{
			m_surfaceO[i] = new mfxFrameSurface1;
			memset(m_surfaceO[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaceO[i]->Info), &(m_videoVPPParam.vpp.Out), sizeof(mfxFrameInfo));
			m_surfaceO[i]->Data.MemId = m_reponses[1].mids[i];
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
		mfxExtBuffer* ExtBuffer[2];
		ExtBuffer[0] = (mfxExtBuffer*)&extDoUse;
		ExtBuffer[1] = (mfxExtBuffer*)&denoiseConfig;
		m_videoVPPParam.NumExtParam = 2;
		m_videoVPPParam.ExtParam = (mfxExtBuffer**)&ExtBuffer[0];
		status = m_videoVPP->Init(&m_videoVPPParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_FILTER_SKIPPED);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}
}