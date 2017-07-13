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
		mfxFrameAllocRequest request;
		memset(&request, 0, sizeof(request));
		status = m_videoDECODE->QueryIOSurf(&m_videoParam, &request);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		request.Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
		if (status != MFX_ERR_NONE)
			return FALSE;
		m_surfaces.Reset(new mfxFrameSurface1 *[m_response.NumFrameActual]);
		if (!m_surfaces)
			return FALSE;
		for (mfxU16 i = 0; i < m_response.NumFrameActual; i++)
		{
			m_surfaces[i] = new mfxFrameSurface1;
			memset(m_surfaces[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaces[i]->Info), &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_surfaces[i]->Data.MemId = m_response.mids[i];
		}
		status = m_videoDECODE->Init(&m_videoParam);
		if (status != MFX_ERR_NONE && status != MFX_WRN_PARTIAL_ACCELERATION)
			return FALSE;
		return TRUE;
	}
	mfxStatus QSV::Process(mfxBitstream& stream, mfxFrameSurface1*& surface1)
	{
		mfxStatus status = MFX_ERR_NONE;
		while (stream.DataLength > 0)
		{
			mfxU16 index = GetFreeSurfaceIndex(m_surfaces, m_response.NumFrameActual);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_videoDECODE->DecodeFrameAsync(&stream, m_surfaces[index], &surface1, &m_syncp);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeSurfaceIndex(m_surfaces, m_response.NumFrameActual);
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
		return status;
	}
	BOOL QSV::Decode(Media::SampleTag& tag, mfxFrameSurface1*& surface1)
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
		mfxStatus status = Process(stream, surface1);
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
	BOOL QSV::Lock(mfxFrameSurface1* surface1)
	{
		if (!surface1)
			return FALSE;
		mfxStatus status = m_allocator.Lock(m_allocator.pthis, surface1->Data.MemId, &(surface1->Data));
		if (status != MFX_ERR_NONE)
			return FALSE;
		return TRUE;
	}
	BOOL QSV::Unlock(mfxFrameSurface1* surface1)
	{
		if (!surface1)
			return FALSE;
		mfxStatus status = m_allocator.Unlock(m_allocator.pthis, surface1->Data.MemId, &(surface1->Data));
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
		for (mfxU16 i = 0; i < m_response.NumFrameActual; i++)
		{
			SAFE_DELETE(m_surfaces[i]);
		}
		m_surfaces.Reset(NULL);
		m_allocator.Free(m_allocator.pthis, &m_response);
		Release();
		return TRUE;
	}
}
