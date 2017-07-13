#include "stdafx.h"
#include "QSV.h"

namespace FLVPlayer
{
	QSV::QSV()
	{
	}

	QSV::~QSV()
	{
	}
	mfxVideoParam& QSV::GetParam()
	{
		return m_videoParam;
	}
	BOOL QSV::Open(const BYTE* bits, LONG size)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		status = ::Initialize(impl, ver, &m_session, &m_allocator);
		if (status != MFX_ERR_NONE)
			return FALSE;
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		if (!m_videoDECODE)
			return FALSE;
		m_videoVPP.Reset(new MFXVideoVPP(m_session));
		if (!m_videoVPP)
			return FALSE;
		m_bits[0].Reset(new BYTE[MAX_STREAM_SIZE]);
		m_bits[1].Reset(new BYTE[MAX_STREAM_SIZE]);
		memset(&m_residial, 0, sizeof(m_residial));
		m_residial.MaxLength = MAX_STREAM_SIZE;
		m_residial.Data = m_bits[1];
		memcpy_s(m_residial.Data, size, bits, size);
		m_residial.DataLength += size;
		memset(&m_videoParam, 0, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		status = m_videoDECODE->DecodeHeader(&m_residial, &m_videoParam);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = m_residial.DataFlag;
			m_residial.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_videoDECODE->DecodeHeader(&m_residial, &m_videoParam);
			m_residial.DataFlag = oldFlag;
		}
		memset(&m_vppParam, 0, sizeof(m_vppParam));
		m_vppParam.vpp.In.FourCC = MFX_FOURCC_NV12;
		m_vppParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParam.vpp.In.CropX = 0;
		m_vppParam.vpp.In.CropY = 0;
		m_vppParam.vpp.In.CropW = m_videoParam.mfx.FrameInfo.CropW;
		m_vppParam.vpp.In.CropH = m_videoParam.mfx.FrameInfo.CropH;
		m_vppParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_vppParam.vpp.In.FrameRateExtN = 30;
		m_vppParam.vpp.In.FrameRateExtD = 1;
		m_vppParam.vpp.In.Width = MSDK_ALIGN16(m_vppParam.vpp.In.CropW);
		m_vppParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParam.vpp.In.PicStruct) ? MSDK_ALIGN16(m_vppParam.vpp.In.CropH) : MSDK_ALIGN32(m_vppParam.vpp.In.CropH);
		m_vppParam.vpp.Out.FourCC = MFX_FOURCC_RGB4;
		m_vppParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_vppParam.vpp.Out.CropX = 0;
		m_vppParam.vpp.Out.CropY = 0;
		m_vppParam.vpp.Out.CropW = m_vppParam.vpp.In.CropW;
		m_vppParam.vpp.Out.CropH = m_vppParam.vpp.In.CropH;
		m_vppParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_vppParam.vpp.Out.FrameRateExtN = 30;
		m_vppParam.vpp.Out.FrameRateExtD = 1;
		m_vppParam.vpp.Out.Width = MSDK_ALIGN16(m_vppParam.vpp.Out.CropW);
		m_vppParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_vppParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_vppParam.vpp.Out.CropH) : MSDK_ALIGN32(m_vppParam.vpp.Out.CropH);
		m_vppParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		mfxFrameAllocRequest request;
		memset(&request, 0, sizeof(request));
		status = m_videoDECODE->QueryIOSurf(&m_videoParam, &request);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		status = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
		if (status != MFX_ERR_NONE)
			return FALSE;
		mfxFrameAllocRequest requests[2];//[0]-IN [1]-OUT
		memset(&requests, 0, sizeof(mfxFrameAllocRequest) * 2);
		status = m_videoVPP->QueryIOSurf(&m_vppParam, requests);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		requests[1].Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &requests[0], &m_responses[0]);
		if (status != MFX_ERR_NONE)
			return FALSE;
		status = m_allocator.Alloc(m_allocator.pthis, &requests[1], &m_responses[1]);
		if (status != MFX_ERR_NONE)
			return FALSE;
		mfxU16 sizeIN = m_response.NumFrameActual + m_responses[0].NumFrameActual;
		mfxU16 sizeOUT = m_responses[1].NumFrameActual;
		m_videoIN.Reset(new mfxFrameSurface1 *[sizeIN]);
		if (!m_videoIN)
			return FALSE;
		m_videoOUT.Reset(new mfxFrameSurface1 *[sizeOUT]);
		if (!m_videoIN)
			return FALSE;
		for (mfxU16 i = 0; i < sizeIN; i++)
		{
			m_videoIN[i] = new mfxFrameSurface1();
			memset(m_videoIN[i], 0, sizeof(mfxFrameSurface1));
			if (i < m_response.NumFrameActual)
			{
				memcpy(&(m_videoIN[i]->Info), &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
				m_videoIN[i]->Data.MemId = m_response.mids[i];
				ClearRGBSurfaceVMem(m_videoIN[i]->Data.MemId);
			}
			else
			{
				memcpy(&(m_videoIN[i]->Info), &(m_vppParam.vpp.In), sizeof(mfxFrameInfo));
				m_videoIN[i]->Data.MemId = m_responses[0].mids[i - m_response.NumFrameActual];
				ClearRGBSurfaceVMem(m_videoIN[i]->Data.MemId);
			}
		}
		for (mfxU16 i = 0; i < sizeOUT; i++)
		{
			m_videoOUT[i] = new mfxFrameSurface1();
			memset(m_videoOUT[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_videoOUT[i]->Info), &(m_vppParam.vpp.Out), sizeof(mfxFrameInfo));
			m_videoOUT[i]->Data.MemId = m_responses[1].mids[i];
			ClearRGBSurfaceVMem(m_videoOUT[i]->Data.MemId);
		}
		status = m_videoDECODE->Init(&m_videoParam);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		status = m_videoVPP->Init(&m_vppParam);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		return TRUE;
	}
	mfxStatus QSV::Process(mfxBitstream& stream, mfxFrameSurface1*& video)
	{
		mfxStatus status = MFX_ERR_NONE;
		while (stream.DataLength > 0)
		{
			mfxFrameSurface1* surface = NULL;
			mfxU16 sizeIN = m_response.NumFrameActual + m_responses[0].NumFrameActual;
			mfxU16 sizeOUT = m_responses[1].NumFrameActual;
			mfxU16 index = GetFreeSurfaceIndex(m_videoIN, sizeIN);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_videoDECODE->DecodeFrameAsync(&stream, m_videoIN[index], &surface, &m_syncpDECODE);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeSurfaceIndex(m_videoIN, sizeIN);
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
			if (MFX_ERR_NONE == status)
			{
				mfxU16 index1 = GetFreeSurfaceIndex(m_videoOUT, sizeOUT);
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
				do
				{
					status = m_videoVPP->RunFrameVPPAsync(surface, m_videoOUT[index1], NULL, &m_syncpVPP);
					if (MFX_ERR_MORE_SURFACE == status)
					{
						index1 = GetFreeSurfaceIndex(m_videoOUT, sizeOUT);
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
					if (status == MFX_ERR_NONE)
					{
						video = m_videoOUT[index1];
					}
					return status;
				}
			}
		}
		return status;
	}
	BOOL QSV::Decode(Media::SampleTag& tag, mfxFrameSurface1*& video)
	{
		mfxBitstream stream;
		memset(&stream, 0, sizeof(stream));
		INT32 newsize = tag.size + m_residial.DataLength;
		stream.MaxLength = stream.DataLength = (mfxU32)newsize;
		stream.TimeStamp = tag.samplePTS;
		stream.Data = m_bits[0];
		mfxU8* pData = stream.Data;
		memcpy(pData, m_residial.Data, m_residial.DataLength);
		pData += m_residial.DataLength;
		m_residial.DataLength = 0;
		memcpy(pData, tag.bits, tag.size);
		pData += tag.size;
		mfxStatus status = Process(stream, video);
		if (status != MFX_ERR_NONE)
			return FALSE;
		m_residial.DataOffset = 0;
		m_residial.DataLength = stream.DataLength;
		if (stream.DataLength > m_residial.MaxLength)
		{
			m_bits[1].Reset(new mfxU8[m_residial.MaxLength]);
			m_residial.MaxLength = stream.DataLength;
			m_residial.Data = m_bits[1];
		}
		memcpy(m_residial.Data, stream.Data + stream.DataOffset, stream.DataLength);
		return TRUE;
	}
	BOOL QSV::Lock(mfxFrameSurface1* video)
	{
		if (!video)
			return FALSE;
		mfxStatus status = m_allocator.Lock(m_allocator.pthis, video->Data.MemId, &(video->Data));
		if (status != MFX_ERR_NONE)
			return FALSE;
		return TRUE;
	}
	BOOL QSV::Unlock(mfxFrameSurface1* video)
	{
		if (!video)
			return FALSE;
		mfxStatus status = m_allocator.Unlock(m_allocator.pthis, video->Data.MemId, &(video->Data));
		if (status != MFX_ERR_NONE)
			return FALSE;
		return TRUE;
	}
	BOOL QSV::Close()
	{
		if (m_videoDECODE != NULL)
		{
			m_videoDECODE->Close();
		}
		if (m_videoVPP != NULL)
		{
			m_videoVPP->Close();
		}
		mfxU16 sizeIN = m_response.NumFrameActual + m_responses[0].NumFrameActual;
		for (mfxU16 i = 0; i < sizeIN; i++)
		{
			SAFE_DELETE(m_videoIN[i]);
		}
		mfxU16 sizeOUT = m_responses[1].NumFrameActual;
		for (mfxU16 i = 0; i < sizeOUT; i++)
		{
			SAFE_DELETE(m_videoOUT[i]);
		}
		m_videoIN.Reset(NULL);
		m_videoOUT.Reset(NULL);
		m_allocator.Free(m_allocator.pthis, &m_response);
		m_allocator.Free(m_allocator.pthis, &m_responses[0]);
		m_allocator.Free(m_allocator.pthis, &m_responses[1]);
		Release();
		return TRUE;
	}
}
