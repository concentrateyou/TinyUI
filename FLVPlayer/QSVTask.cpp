#include "stdafx.h"
#include "QSVTask.h"
#include "QSVDecode.h"

namespace FLVPlayer
{
	QSVTask::QSVTask()
		:m_hFile(NULL),
		m_size(NULL)
	{

	}
	QSVTask::~QSVTask()
	{
		fclose(m_hFile);
	}
	BOOL QSVTask::Initialize(HWND hWND)
	{
		if (!m_reader.OpenFile("D:\\File\\3.flv"))
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
			mfxU16 index = GetFreeSurfaceIndex(m_pmfxSurfaces, m_response.NumFrameActual);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_videoDECODE->DecodeFrameAsync(&stream, m_pmfxSurfaces[index], &surface1, &m_syncp);
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
		return status;
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
						memset(&m_videoParam, 0, sizeof(m_videoParam));
						m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
						m_videoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;

						m_bits.Reset(new BYTE[MAX_STREAM_SIZE]);
						MSDK_CHECK_POINTER(m_bits, MFX_ERR_MEMORY_ALLOC);

						memset(&m_residial, 0, sizeof(m_residial));
						m_residial.MaxLength = MAX_STREAM_SIZE;
						m_residial.Data = new mfxU8[m_residial.MaxLength];
						MSDK_CHECK_POINTER(m_residial.Data, MFX_ERR_MEMORY_ALLOC);


						memcpy_s(m_residial.Data, block.video.size, block.video.data, block.video.size);
						m_residial.DataLength += block.video.size;
						mfxStatus status = m_videoDECODE->DecodeHeader(&m_residial, &m_videoParam);
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
						MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
						MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
						request.Type |= WILL_READ;
						status = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
						MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
						m_pmfxSurfaces = new mfxFrameSurface1 *[m_response.NumFrameActual];
						MSDK_CHECK_POINTER(m_pmfxSurfaces, MFX_ERR_MEMORY_ALLOC);
						for (mfxU16 i = 0; i < m_response.NumFrameActual; i++)
						{
							m_pmfxSurfaces[i] = new mfxFrameSurface1;
							memset(m_pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
							memcpy(&(m_pmfxSurfaces[i]->Info), &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
							m_pmfxSurfaces[i]->Data.MemId = m_response.mids[i];
						}
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