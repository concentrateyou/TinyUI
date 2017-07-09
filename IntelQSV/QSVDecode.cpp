#include "stdafx.h"
#include "QSVDecode.h"

namespace QSV
{
	QSVDecode::QSVDecode()
		:m_surface(NULL)
	{
	}


	QSVDecode::~QSVDecode()
	{
	}
	mfxStatus QSVDecode::Initialize()
	{
		memset(&m_videoParams, 0, sizeof(m_videoParams));
		m_videoParams.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = ::Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = 1024 * 1024;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		return status;
	}
	mfxStatus QSVDecode::Initialize(Callback<void(BYTE*, LONG, INT)>&& callback)
	{
		m_callback = std::move(callback);
		memset(&m_videoParams, 0, sizeof(m_videoParams));
		m_videoParams.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = ::Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = 1024 * 1024;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		return status;
	}
	mfxStatus QSVDecode::Open(BYTE* bits, LONG size)
	{
		mfxBitstream bs;
		memset(&bs, 0, sizeof(bs));
		bs.DataLength = size;
		bs.Data = bits;
		bs.MaxLength = size;
		mfxStatus status = m_videoDECODE->DecodeHeader(&bs, &m_videoParams);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = bs.DataFlag;
			bs.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_videoDECODE->DecodeHeader(&bs, &m_videoParams);
			bs.DataFlag = oldFlag;
		}
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxBitstream bs2 = bs;
			bs2.Data = new mfxU8[bs.DataLength + 5];
			memcpy(bs2.Data, bs.Data + bs.DataOffset, bs.DataLength - bs.DataOffset);
			bs2.MaxLength = bs2.DataLength = bs.DataLength + 5 - bs.DataOffset;
			*((unsigned*)(bs2.Data + bs2.DataLength - 5)) = 0x01000000;
			bs2.Data[bs2.DataLength - 1] = 1;
			status = m_videoDECODE->DecodeHeader(&bs2, &m_videoParams);
			delete[] bs2.Data;
		}
		memcpy_s(m_bitstream.Data, size, bs.Data, size);
		m_bitstream.DataLength += size;
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_allocRequest, 0, sizeof(m_allocRequest));
		status = m_videoDECODE->QueryIOSurf(&m_videoParams, &m_allocRequest);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_allocRequest.Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &m_allocRequest, &m_allocResponse);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_surfaces.Reset(new mfxFrameSurface1 *[m_allocRequest.NumFrameSuggested]);
		MSDK_CHECK_POINTER(m_surfaces, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_allocRequest.NumFrameSuggested; i++)
		{
			m_surfaces[i] = new mfxFrameSurface1;
			memset(m_surfaces[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaces[i]->Info), &(m_videoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_surfaces[i]->Data.MemId = m_allocResponse.mids[i];
		}
		status = m_videoDECODE->Init(&m_videoParams);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		INT index = GetFreeSurfaceIndex(m_surfaces, m_allocRequest.NumFrameSuggested);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
		m_videoDECODE->DecodeFrameAsync(&m_bitstream, m_surfaces[index], &m_surface, &m_syncp);
		return status;
	}

	mfxStatus QSVDecode::Decode(BYTE* bits, LONG size, LONGLONG timespan)
	{
		if (!m_videoDECODE)
			return MFX_ERR_NULL_PTR;
		m_bitstream.TimeStamp = timespan;
		memcpy_s(m_bitstream.Data + m_bitstream.DataOffset, size, bits, size);
		m_bitstream.DataLength += size;
		INT index = GetFreeSurfaceIndex(m_surfaces, m_allocRequest.NumFrameSuggested);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
		mfxStatus status = m_videoDECODE->DecodeFrameAsync(&m_bitstream, m_surfaces[index], &m_surface, &m_syncp);
		if (MFX_ERR_NONE < status && m_syncp)
		{
			status = MFX_ERR_NONE;
		}
		if (MFX_ERR_NONE == status)
		{
			status = m_session.SyncOperation(m_syncp, 60000);
		}
		if (MFX_ERR_NONE == status)
		{
			status = m_allocator.Lock(m_allocator.pthis, m_surface->Data.MemId, &(m_surface->Data));
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			if (!m_callback.IsNull())
			{
				m_callback(m_bitstream.Data + m_bitstream.DataOffset, m_bitstream.DataLength, m_bitstream.FrameType);
			}
			status = m_allocator.Unlock(m_allocator.pthis, m_surface->Data.MemId, &(m_surface->Data));
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		}
		return status;
	}

	mfxStatus QSVDecode::Close()
	{
		if (m_videoDECODE)
		{
			m_videoDECODE->Close();
		}
		for (INT i = 0; i < m_allocRequest.NumFrameSuggested; i++)
		{
			delete m_surfaces[i];
		}
		m_surfaces.Reset(NULL);
		MSDK_SAFE_DELETE_ARRAY(m_bitstream.Data);
		m_allocator.Free(m_allocator.pthis, &m_allocResponse);
		Release();
		return MFX_ERR_NONE;
	}
}
