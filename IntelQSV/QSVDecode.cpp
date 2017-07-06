#include "stdafx.h"
#include "QSVDecode.h"

namespace QSV
{
	QSVDecode::QSVDecode()
	{
	}


	QSVDecode::~QSVDecode()
	{
	}

	mfxStatus QSVDecode::Open(BYTE* bits, LONG size, Callback<void(BYTE*, LONG, INT)>&& callback)
	{
		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		mfxStatus status = Initialize(impl, ver, &m_session, &m_allocator);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_videoDECODE.Reset(new MFXVideoDECODE(m_session));
		MSDK_CHECK_POINTER(m_videoDECODE, MFX_ERR_MEMORY_ALLOC);
		mfxVideoParam mfxVideoParams;
		memset(&mfxVideoParams, 0, sizeof(mfxVideoParams));
		mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;
		mfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		memset(&m_bitstream, 0, sizeof(m_bitstream));
		m_bitstream.MaxLength = 1024 * 1024;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		MSDK_CHECK_POINTER(m_bitstream.Data, MFX_ERR_MEMORY_ALLOC);
		ASSERT(m_bitstream.MaxLength >= size);
		memcpy_s(m_bitstream.Data, size, bits, size);
		status = m_videoDECODE->DecodeHeader(&m_bitstream, &mfxVideoParams);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_allocRequest, 0, sizeof(m_allocRequest));
		status = m_videoDECODE->QueryIOSurf(&mfxVideoParams, &m_allocRequest);
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
			memcpy(&(m_surfaces[i]->Info), &(mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_surfaces[i]->Data.MemId = m_allocResponse.mids[i];
		}
		status = m_videoDECODE->Init(&mfxVideoParams);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_callback = std::move(callback);
		return status;
	}

	mfxStatus QSVDecode::Decode(BYTE* data, LONG size)
	{
		if (!m_videoDECODE)
			return MFX_ERR_NULL_PTR;
		mfxSyncPoint syncp;
		mfxFrameSurface1* pmfxOutSurface = NULL;
		mfxStatus status = MFX_ERR_NONE;
		ASSERT(m_bitstream.MaxLength >= size);
		m_bitstream.DataLength = size;
		memcpy_s(m_bitstream.Data, size, data, size);
		INT index = GetFreeSurfaceIndex(m_surfaces, m_allocRequest.NumFrameSuggested);
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
		status = m_videoDECODE->DecodeFrameAsync(&m_bitstream, m_surfaces[index], &pmfxOutSurface, &syncp);
		if (MFX_ERR_NONE < status && syncp)
		{
			status = MFX_ERR_NONE;
		}
		if (MFX_ERR_NONE == status)
		{
			status = m_session.SyncOperation(syncp, 60000);
		}
		if (MFX_ERR_NONE == status)
		{
			status = m_allocator.Lock(m_allocator.pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
			MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
			if (!m_callback.IsNull())
			{
				m_callback(m_bitstream.Data + m_bitstream.DataOffset, m_bitstream.DataLength, m_bitstream.FrameType);
			}
			status = m_allocator.Unlock(m_allocator.pthis, pmfxOutSurface->Data.MemId, &(pmfxOutSurface->Data));
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
