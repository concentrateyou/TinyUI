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
		memset(&m_videoParams, 0, sizeof(m_videoParams));
		m_videoParams.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		memcpy_s(m_bitstream.Data, size, bits, size);
		m_bitstream.DataLength += size;
		mfxStatus status = m_videoDECODE->DecodeHeader(&m_bitstream, &m_videoParams);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = m_bitstream.DataFlag;
			m_bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_videoDECODE->DecodeHeader(&m_bitstream, &m_videoParams);
			m_bitstream.DataFlag = oldFlag;
		}
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		memset(&m_request, 0, sizeof(m_request));
		status = m_videoDECODE->QueryIOSurf(&m_videoParams, &m_request);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_request.Type |= WILL_READ;
		status = m_allocator.Alloc(m_allocator.pthis, &m_request, &m_response);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		m_decodeSF.Reset(new mfxFrameSurface1 *[m_request.NumFrameSuggested]);
		MSDK_CHECK_POINTER(m_decodeSF, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_request.NumFrameSuggested; i++)
		{
			m_decodeSF[i] = new mfxFrameSurface1();
			memset(m_decodeSF[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_decodeSF[i]->Info), &(m_videoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_decodeSF[i]->Data.MemId = m_response.mids[i];
		}
		status = m_videoDECODE->Init(&m_videoParams);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	mfxStatus QSVDecode::Decode(BYTE* bits, LONG size, LONGLONG timespan)
	{
		if (!m_videoDECODE)
			return MFX_ERR_NULL_PTR;
		mfxStatus status = MFX_ERR_NONE;
		m_bitstream.TimeStamp = timespan;
		mfxU32 value = m_bitstream.MaxLength - m_bitstream.DataOffset;
		if (value < size)
		{
			memmove_s(m_bitstream.Data, m_bitstream.DataLength, m_bitstream.Data + m_bitstream.DataLength, m_bitstream.DataLength);
			m_bitstream.DataOffset = 0;
			value = min(size, m_bitstream.MaxLength - m_bitstream.DataLength);
			memcpy_s(m_bitstream.Data + m_bitstream.DataLength, value, bits, value);
		}
		else
		{
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
		while (MFX_ERR_NONE <= status || MFX_ERR_MORE_DATA == status || MFX_ERR_MORE_SURFACE == status)
		{
			if (MFX_WRN_DEVICE_BUSY == status)
				MSDK_SLEEP(1);
			if (MFX_ERR_MORE_DATA == status)
				break;
			if (MFX_ERR_MORE_SURFACE == status || MFX_ERR_NONE == status)
			{
				index = GetFreeSurfaceIndex(m_decodeSF, m_request.NumFrameSuggested);
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			}
			status = m_videoDECODE->DecodeFrameAsync(&m_bitstream, m_decodeSF[index], &m_currentSF, &m_syncPoint);
			if (MFX_ERR_NONE < status && m_syncPoint)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_session.SyncOperation(m_syncPoint, INFINITE);
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_allocator.Lock(m_allocator.pthis, m_currentSF->Data.MemId, &(m_currentSF->Data));
				MSDK_BREAK_ON_ERROR(status);
				WriteRawFrame(m_currentSF, m_hFile);
				status = m_allocator.Unlock(m_allocator.pthis, m_currentSF->Data.MemId, &(m_currentSF->Data));
				MSDK_BREAK_ON_ERROR(status);
			}
		}
		return status;
	}

	mfxStatus QSVDecode::Close()
	{
		if (m_videoDECODE != NULL)
			m_videoDECODE->Close();
		for (INT i = 0; i < m_request.NumFrameSuggested; i++)
		{
			SAFE_DELETE(m_decodeSF[i]);
		}
		m_decodeSF.Reset(NULL);
		MSDK_SAFE_DELETE_ARRAY(m_bitstream.Data);
		m_allocator.Free(m_allocator.pthis, &m_response);
		Release();
		return MFX_ERR_NONE;
	}
}
