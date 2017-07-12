#include "stdafx.h"
#include "QSVTask.h"
#include "QSVDecode.h"

namespace FLVPlayer
{
	QSVTask::QSVTask()
		:m_hFile(NULL)
	{

	}
	QSVTask::~QSVTask()
	{
		fclose(m_hFile);
	}
	BOOL QSVTask::Initialize(HWND hWND)
	{
		if (!m_reader.OpenFile("D:\\2.flv"))
			return FALSE;
		TinySize size(static_cast<LONG>(m_reader.GetScript().width), static_cast<LONG>(m_reader.GetScript().height));
		TinyRectangle rectangle;
		::GetClientRect(hWND, &rectangle);
		if (!m_graphics.Initialize(hWND, rectangle.Size()))
			return FALSE;
		if (!m_video2D.Create(m_graphics.GetDX11(), size, TRUE))
			return FALSE;
		m_video2D.SetScale(rectangle.Size());
		return TRUE;
	}
	BOOL QSVTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&QSVTask::OnMessagePump, this));
	}
	BOOL QSVTask::Close(DWORD dwMS)
	{
		return TinyTaskBase::Close(dwMS);
	}

	mfxStatus QSVTask::OnVideo1(mfxBitstream& stream, mfxFrameSurface1*& surface1)
	{
		mfxStatus status = MFX_ERR_NONE;
		while (stream.DataLength > 0)
		{
			mfxU32 count = m_response.NumFrameActual + m_reponses[0].NumFrameActual;
			mfxU16 index = GetFreeSurfaceIndex(m_videoISF, count);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_videoDECODE->DecodeFrameAsync(&stream, m_videoISF[index], &surface1, &m_syncpDECODE);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeSurfaceIndex(m_videoISF, count);
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
				}
				if (MFX_WRN_DEVICE_BUSY == status)
				{
					MSDK_SLEEP(1);
				}
			} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
			if (MFX_ERR_NONE < status && m_syncpDECODE)
			{
				status = MFX_ERR_NONE;
			}
			mfxU16 index1 = GetFreeSurfaceIndex(m_videoOSF, m_reponses[1].NumFrameActual);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
			do 
			{
				status = m_videoVPP->RunFrameVPPAsync(m_currentSF, m_videoOSF[index1], NULL, &m_syncpVPP);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index1 = GetFreeSurfaceIndex(m_videoOSF, m_reponses[1].NumFrameActual);
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
				}
				if (MFX_WRN_DEVICE_BUSY == status)
				{
					MSDK_SLEEP(1);
				}
			} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
			if (MFX_ERR_NONE < status && m_syncpVPP)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_session.SyncOperation(m_syncpVPP, 60000);
			}
		}
		/*mfxStatus status = MFX_ERR_NONE;
		while (stream.DataLength > 0)
		{
			mfxU16 index = GetFreeSurfaceIndex(m_pmfxSurfaces, m_response.NumFrameActual);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_videoDECODE->DecodeFrameAsync(&stream, m_videoISF[index], &surface1, &m_syncp);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeSurfaceIndex(m_pmfxSurfaces, m_response.NumFrameActual);
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
				}
				if (MFX_WRN_DEVICE_BUSY == status)
				{
					MSDK_SLEEP(1);
				}
			} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
			if (MFX_ERR_NONE < status && m_syncp)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				return m_session.SyncOperation(m_syncp, 60000);
			}
		}
		return status;*/
	}

	mfxStatus QSVTask::OnVideo(const BYTE* bi, LONG si, LONGLONG timestamp, mfxFrameSurface1*& surface1)
	{
		mfxBitstream stream;
		memset(&stream, 0, sizeof(stream));
		INT32 newsize = si + m_residial.DataLength;
		stream.MaxLength = stream.DataLength = (mfxU32)newsize;
		stream.TimeStamp = timestamp;
		stream.Data = m_bits;
		mfxU8* pData = stream.Data;
		memcpy(pData, m_residial.Data, m_residial.DataLength);
		pData += m_residial.DataLength;
		m_residial.DataLength = 0;
		memcpy(pData, bi, si);
		pData += si;

		mfxStatus status = OnVideo1(stream, surface1);
		if (status != MFX_ERR_NONE)
			goto _ERROR;
		m_residial.DataOffset = 0;
		m_residial.DataLength = stream.DataLength;
		if (stream.DataLength > m_residial.MaxLength)
		{
			SAFE_DELETE_ARRAY(m_residial.Data);
			mfxU32 newSize = stream.DataLength;
			m_residial.Data = new mfxU8[newSize];
			m_residial.MaxLength = newSize;
		}
		memcpy(m_residial.Data, stream.Data + stream.DataOffset, stream.DataLength);

		status = m_allocator.Lock(m_allocator.pthis, surface1->Data.MemId, &(surface1->Data));
		if (status != MFX_ERR_NONE)
			goto _ERROR;
		status = WriteRawFrame(surface1, m_hFile);
		if (status != MFX_ERR_NONE)
			goto _ERROR;
		status = m_allocator.Unlock(m_allocator.pthis, surface1->Data.MemId, &(surface1->Data));
		if (status != MFX_ERR_NONE)
			goto _ERROR;
	_ERROR:
		Sleep(15);
		return status;
	}

	mfxStatus QSVTask::OnInvoke()
	{
		MSDK_FOPEN(m_hFile, "D:\\test1.yuv", "wb");
		MSDK_CHECK_POINTER(m_hFile, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		status = ::Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (!m_reader.ReadBlock(block))
				break;
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
						mfxVersion ver = { { 0, 1 } };
						mfxStatus status = ::Initialize(impl, ver, &m_session, &m_allocator);
						MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
						m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
						MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
						memset(&m_videoParam, 0, sizeof(m_videoParam));
						m_residial.mfx.CodecId = MFX_CODEC_AVC;
						m_residial.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
						memcpy_s(m_residial.Data, block.video.size, block.video.data, block.video.size);
						m_residial.DataLength += block.video.size;
						mfxStatus status = m_videoDECODE->DecodeHeader(&m_bitstream, &m_videoParam);
						if (MFX_ERR_MORE_DATA == status)
						{
							mfxU16 oldFlag = m_residial.DataFlag;
							m_residial.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
							status = m_videoDECODE->DecodeHeader(&m_residial, &m_videoParam);
							m_residial.DataFlag = oldFlag;
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
						m_vppParam.vpp.In.FourCC = MFX_FOURCC_NV12;
						m_vppParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
						m_vppParam.vpp.In.PicStruct = m_videoParam.vpp.In.PicStruct;
						m_vppParam.vpp.In.FrameRateExtN = m_videoParam.vpp.In.FrameRateExtN;
						m_vppParam.vpp.In.FrameRateExtD = m_videoParam.vpp.In.FrameRateExtD;
						m_vppParam.vpp.In.CropW = m_videoParam.vpp.In.CropW;
						m_vppParam.vpp.In.CropH = m_videoParam.vpp.In.CropH;
						m_vppParam.vpp.In.Width = m_videoParam.vpp.In.Width;
						m_vppParam.vpp.In.Height = m_videoParam.vpp.In.Height;
						m_vppParam.vpp.Out.FourCC = MFX_FOURCC_RGB4;
						m_vppParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
						m_vppParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
						m_vppParam.vpp.Out.FrameRateExtN = m_videoParam.vpp.In.FrameRateExtN;
						m_vppParam.vpp.Out.FrameRateExtD = m_videoParam.vpp.In.FrameRateExtD;
						m_vppParam.vpp.Out.CropW = m_videoParam.vpp.In.CropW;
						m_vppParam.vpp.Out.CropH = m_videoParam.vpp.In.CropH;
						m_vppParam.vpp.Out.Width = m_videoParam.vpp.In.Width;
						m_vppParam.vpp.Out.Height = m_videoParam.vpp.In.Height;
						m_vppParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
						m_videoVPP.Reset(new MFXVideoVPP(m_session));
						MSDK_CHECK_POINTER(m_videoVPP, MFX_ERR_MEMORY_ALLOC);

						mfxFrameAllocRequest requests[2];
						memset(&requests, 0, sizeof(mfxFrameAllocRequest) * 2);
						status = m_videoVPP->QueryIOSurf(&m_vppParam, requests);
						MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
						requests[0].Type |= WILL_WRITE;
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
					}
					if (block.video.packetType == FLV_NALU)
					{
						mfxFrameSurface1* ps = NULL;
						OnVideo(block.video.data, block.video.size, block.pts, ps);
					}
					SAFE_DELETE_ARRAY(block.audio.data);
					SAFE_DELETE_ARRAY(block.video.data);
					Sleep(15);
				}
			}
		}

		MSDK_IGNORE_MFX_STS(status, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	void QSVTask::OnMessagePump()
	{
		OnInvoke();
	}
}