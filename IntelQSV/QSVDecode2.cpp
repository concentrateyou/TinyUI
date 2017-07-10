#include "stdafx.h"
#include "QSVDecode.h"

namespace QSV
{
	QSVDecode::QSVDecode()
		:m_currentSF(NULL),
		m_hFile(NULL)
	{
		fopen_s(&m_hFile, "D:\\test1.yuv", "wb");
	}


	QSVDecode::~QSVDecode()
	{
		fclose(m_hFile);
	}
	mfxStatus QSVDecode::Initialize()
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = ::Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = 1024 * 1024;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		return status;
	}

	mfxStatus QSVDecode::Open(BYTE* bits, LONG size)
	{
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
		memset(&m_videoParam, 0, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		memcpy_s(m_bitstream.Data, size, bits, size);
		m_bitstream.DataLength += size;
		mfxStatus status = m_videoDECODE->DecodeHeader(&m_bitstream, &m_videoParam);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = m_bitstream.DataFlag;
			m_bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_videoDECODE->DecodeHeader(&m_bitstream, &m_videoParam);
			m_bitstream.DataFlag = oldFlag;
		}
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);

		mfxFrameAllocRequest request;
		memset(&request, 0, sizeof(request));
		status = m_videoDECODE->QueryIOSurf(&m_videoParam, &request);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		request.Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		//VPP
		ZeroMemory(&m_vppParam, sizeof(m_vppParam));
		m_vppParam.vpp.In.FourCC = MFX_FOURCC_RGB4;
		m_vppParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_vppParam.vpp.In.FrameRateExtN = 30;
		m_vppParam.vpp.In.FrameRateExtD = 1;
		m_vppParam.vpp.In.CropW = m_videoParam.mfx.FrameInfo.CropW;
		m_vppParam.vpp.In.CropH = m_videoParam.mfx.FrameInfo.CropH;
		m_vppParam.vpp.In.Width = MSDK_ALIGN16(m_vppParam.vpp.In.CropW);
		m_vppParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParam.vpp.In.PicStruct) ? MSDK_ALIGN16(m_vppParam.vpp.In.CropH) : MSDK_ALIGN32(m_vppParam.vpp.In.CropW);
		m_vppParam.vpp.Out.FourCC = MFX_FOURCC_NV12;
		m_vppParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_vppParam.vpp.Out.FrameRateExtN = 30;
		m_vppParam.vpp.Out.FrameRateExtD = 1;
		m_vppParam.vpp.Out.CropW = m_videoParam.mfx.FrameInfo.CropW;
		m_vppParam.vpp.Out.CropH = m_videoParam.mfx.FrameInfo.CropH;
		m_vppParam.vpp.Out.Width = MSDK_ALIGN16(m_vppParam.vpp.Out.CropW);
		m_vppParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_vppParam.vpp.Out.CropH) : MSDK_ALIGN32(m_vppParam.vpp.Out.CropH);
		m_vppParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		m_videoVPP.Reset(new MFXVideoVPP(m_session));
		MSDK_CHECK_POINTER(m_videoVPP, MFX_ERR_MEMORY_ALLOC);

		mfxFrameAllocRequest requests[2];
		memset(&requests, 0, sizeof(mfxFrameAllocRequest) * 2);
		status = m_videoVPP->QueryIOSurf(&m_vppParam, requests);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		requests[0].Type |= WILL_READ;
		requests[1].Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &requests[0], &m_reponses[0]);//IN
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		status = m_allocator.Alloc(m_allocator.pthis, &requests[1], &m_reponses[1]);//OUT
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		mfxU32 count = m_response.NumFrameActual + m_reponses[0].NumFrameActual;
		m_videoISF.Reset(new mfxFrameSurface1 *[count]);
		MSDK_CHECK_POINTER(m_videoISF, MFX_ERR_MEMORY_ALLOC);
		for (mfxU16 i = 0; i < m_response.NumFrameActual; i++)
		{
			m_videoISF[i] = new mfxFrameSurface1();
			memset(m_videoISF[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_videoISF[i]->Info), &(m_vppParam.vpp.In), sizeof(mfxFrameInfo));
			m_videoISF[i]->Data.MemId = m_response.mids[i];
		}
		for (mfxU16 i = m_response.NumFrameActual; i < count; i++)
		{
			m_videoISF[i] = new mfxFrameSurface1();
			memset(m_videoISF[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_videoISF[i]->Info), &(m_vppParam.vpp.In), sizeof(mfxFrameInfo));
			m_videoISF[i]->Data.MemId = m_reponses[0].mids[i - m_response.NumFrameActual];
		}
		m_videoOSF.Reset(new mfxFrameSurface1 *[m_reponses[1].NumFrameActual]);
		MSDK_CHECK_POINTER(m_videoOSF, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_reponses[1].NumFrameActual; i++)
		{
			m_videoOSF[i] = new mfxFrameSurface1();
			memset(m_videoOSF[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_videoOSF[i]->Info), &(m_vppParam.vpp.Out), sizeof(mfxFrameInfo));
			m_videoOSF[i]->Data.MemId = m_reponses[1].mids[i];
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
		m_vppParam.NumExtParam = 2;
		m_vppParam.ExtParam = (mfxExtBuffer**)&ExtBuffer[0];
		status = m_videoVPP->Init(&m_vppParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_FILTER_SKIPPED);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);

		status = m_videoDECODE->Init(&m_videoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	mfxStatus QSVDecode::Decode(BYTE* bits, LONG size, LONGLONG timespan)
	{
		if (!m_videoDECODE || !m_videoVPP)
			return MFX_ERR_NULL_PTR;
		mfxStatus status = MFX_ERR_NONE;
		m_bitstream.TimeStamp = timespan;
		mfxU32 value = m_bitstream.MaxLength - m_bitstream.DataOffset;
		if (value < size)
		{
			memmove_s(m_bitstream.Data, m_bitstream.DataLength, m_bitstream.Data + m_bitstream.DataLength, m_bitstream.DataLength);
			m_bitstream.DataOffset = 0;
			ASSERT((m_bitstream.DataLength + size) <= m_bitstream.MaxLength);
			value = min(size, m_bitstream.MaxLength - m_bitstream.DataLength);
			memcpy_s(m_bitstream.Data + m_bitstream.DataLength, value, bits, value);
		}
		else
		{
			ASSERT((m_bitstream.DataLength + size) <= m_bitstream.MaxLength);
			if (m_bitstream.DataLength > 0)
			{
				memcpy_s(m_bitstream.Data + m_bitstream.DataLength, size, bits, size);
			}
			else
			{
				memcpy_s(m_bitstream.Data + m_bitstream.DataOffset, size, bits, size);
			}
		}
		m_bitstream.DataLength += size;
		INT index = 0;
		INT index1 = 0;
		while (MFX_ERR_NONE <= status || MFX_ERR_MORE_DATA == status || MFX_ERR_MORE_SURFACE == status)
		{
			if (MFX_WRN_DEVICE_BUSY == status)
				MSDK_SLEEP(1);
			if (MFX_ERR_MORE_DATA == status)
				break;
			if (MFX_ERR_MORE_SURFACE == status || MFX_ERR_NONE == status)
			{
				mfxU32 count = m_response.NumFrameActual + m_reponses[0].NumFrameActual;
				index = GetFreeSurfaceIndex(m_videoISF, count);
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			}
			status = m_videoDECODE->DecodeFrameAsync(&m_bitstream, m_videoISF[index], &m_currentSF, &m_syncDECODE);
			if (MFX_ERR_NONE < status && m_syncDECODE)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_session.SyncOperation(m_syncDECODE, 60000);
			}
			if (MFX_ERR_NONE == status)
			{
				index1 = GetFreeSurfaceIndex(m_videoISF, m_reponses[0].NumFrameActual);
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
				for (;;)
				{
					status = m_videoVPP->RunFrameVPPAsync(m_currentSF, m_videoOSF[index1], NULL, &m_syncpVPP);

					if (MFX_ERR_NONE < status && !m_syncpVPP)
					{
						if (MFX_WRN_DEVICE_BUSY == status)
							MSDK_SLEEP(1);
					}
					else if (MFX_ERR_NONE < status && m_syncpVPP)
					{
						status = MFX_ERR_NONE;
						break;
					}
					else
						break;
				}
			}
		}
		return status;
	}

	mfxStatus QSVDecode::Close()
	{
		if (m_videoDECODE != NULL)
			m_videoDECODE->Close();
		if (m_videoVPP != NULL)
			m_videoVPP->Close();
		mfxU32 count = m_response.NumFrameActual + m_reponses[0].NumFrameActual;
		for (INT i = 0; i < count; i++)
		{
			SAFE_DELETE(m_videoISF[i]);
		}
		for (INT i = 0; i < m_reponses[1].NumFrameActual; i++)
		{
			SAFE_DELETE(m_videoOSF[i]);
		}
		m_videoISF.Reset(NULL);
		m_videoOSF.Reset(NULL);
		MSDK_SAFE_DELETE_ARRAY(m_bitstream.Data);
		m_allocator.Free(m_allocator.pthis, &m_response);
		m_allocator.Free(m_allocator.pthis, &m_reponses[0]);
		m_allocator.Free(m_allocator.pthis, &m_reponses[1]);
		Release();
		return MFX_ERR_NONE;
	}
}
