#include "stdafx.h"
#include "QuickSyncDecoder.h"

namespace QSV
{
	QuickSyncDecoder::QuickSyncDecoder()
		:m_bAllowD3D(FALSE),
		m_bAllowD3D11(FALSE),
		m_bHDW(FALSE),
		m_assist(8)
	{
		MSDK_ZERO_MEMORY(&m_mfxVideoParam, sizeof(m_mfxVideoParam));
	}
	QuickSyncDecoder::~QuickSyncDecoder()
	{
		Close();
	}
	mfxStatus QuickSyncDecoder::InitializeVideoParam(BYTE* bits, LONG size)
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		memset(&m_mfxVideoParam, 0, sizeof(m_mfxVideoParam));
		m_mfxVideoParam.mfx.CodecId = MFX_CODEC_AVC;
		memcpy_s(m_mfxResidial.Data, size, bits, size);
		m_mfxResidial.DataLength += size;
		status = m_mfxVideoDECODE->DecodeHeader(&m_mfxResidial, &m_mfxVideoParam);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = m_mfxResidial.DataFlag;
			m_mfxResidial.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_mfxVideoDECODE->DecodeHeader(&m_mfxResidial, &m_mfxVideoParam);
			m_mfxResidial.DataFlag = oldFlag;
		}
		if (MFX_ERR_MORE_DATA == status && m_mfxVideoParam.mfx.CodecId == MFX_CODEC_AVC)
		{
			mfxBitstream bs2 = m_mfxResidial;
			bs2.DataOffset = 0;
			bs2.Data = new mfxU8[m_mfxResidial.DataLength + 5];
			memcpy(bs2.Data, m_mfxResidial.Data + m_mfxResidial.DataOffset, m_mfxResidial.DataLength);
			bs2.MaxLength = bs2.DataLength = m_mfxResidial.DataLength + 5;
			*((unsigned*)(bs2.Data + bs2.DataLength - 5)) = 0x01000000;
			bs2.Data[bs2.DataLength - 1] = 1;
			status = m_mfxVideoDECODE->DecodeHeader(&bs2, &m_mfxVideoParam);
			SAFE_DELETE_ARRAY(bs2.Data);
		}
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		return status;
	}
	mfxStatus QuickSyncDecoder::CreateAllocator()
	{
		MSDK_CHECK_POINTER(m_mfxVideoSession, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		TinyScopedPtr<mfxAllocatorParams> allocatorParams;
		if (m_bAllowD3D)
		{
			POINT pos = { 0 };
			HWND hWND = ::WindowFromPoint(pos);
			if (!hWND)
			{
				hWND = GetDesktopWindow();
			}
			m_mfxVideoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY | MFX_IOPATTERN_IN_VIDEO_MEMORY;
			INT adapterID = GetMSDKAdapters(*m_mfxVideoSession);
			if (adapterID < 0)
			{
				status = MFX_ERR_UNSUPPORTED;
				return status;
			}
			if (m_bAllowD3D11)
			{
#if MFX_D3D11_SUPPORT	
				m_qsvd3d.Reset(new QSVD3D11());
				MSDK_CHECK_POINTER(m_qsvd3d, MFX_ERR_MEMORY_ALLOC);
				status = m_qsvd3d->Initialize((mfxHDL)hWND, adapterID);
				MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
				mfxHDL hdl = m_qsvd3d->GetHandle(MFX_HANDLE_D3D11_DEVICE);
				status = m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D11_DEVICE, hdl);
				MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
				QSVD3D11AllocatorParams* params = new QSVD3D11AllocatorParams();
				MSDK_CHECK_POINTER(params, MFX_ERR_MEMORY_ALLOC);
				m_qsvd3d->pDevice = (ID3D11Device*)m_qsvd3d->GetHandle(MFX_HANDLE_D3D11_DEVICE);
				allocatorParams.Reset(params);
				m_allocator.Reset(new QSVD3D11Allocator());
				MSDK_CHECK_POINTER(m_allocator, MFX_ERR_MEMORY_ALLOC);
#endif
			}
			else
			{
				m_qsvd3d.Reset(new QSVD3D9());
				MSDK_CHECK_POINTER(m_qsvd3d, MFX_ERR_MEMORY_ALLOC);
				status = m_qsvd3d->Initialize((mfxHDL)hWND, adapterID);
				MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
				mfxHDL hdl = m_qsvd3d->GetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER);
				status = m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, hdl);
				MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
				QSVD3D9AllocatorParams* params = new QSVD3D9AllocatorParams();
				MSDK_CHECK_POINTER(params, MFX_ERR_MEMORY_ALLOC);
				params->pManager = (IDirect3DDeviceManager9*)hdl;
				allocatorParams.Reset(params);
				m_allocator.Reset(new QSVD3D9Allocator());
				MSDK_CHECK_POINTER(m_allocator, MFX_ERR_MEMORY_ALLOC);
			}
		}
		else
		{
			m_mfxVideoParam.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY | MFX_IOPATTERN_IN_SYSTEM_MEMORY;
			m_allocator.Reset(new QSVMemeryAllocator());
			MSDK_CHECK_POINTER(m_allocator, MFX_ERR_MEMORY_ALLOC);
		}
		status = m_allocator->Initialize(allocatorParams);
		MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
		status = m_mfxVideoSession->SetFrameAllocator(m_allocator);
		MSDK_CHECK_RESULT_P_RET(MFX_ERR_NONE, status);
		return status;
	}
	mfxStatus QuickSyncDecoder::AllocFrames()
	{
		MSDK_CHECK_POINTER(m_allocator, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_mfxVideoSession, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxFrameAllocRequest allocRequest;
		MSDK_ZERO_MEMORY(&allocRequest, sizeof(allocRequest));
		m_mfxVideoParam.mfx.FrameInfo.Width = MSDK_ALIGN32(m_mfxVideoParam.mfx.FrameInfo.Width);
		m_mfxVideoParam.mfx.FrameInfo.Height = MSDK_ALIGN32(m_mfxVideoParam.mfx.FrameInfo.Height);
		mfxStatus status = m_mfxVideoDECODE->QueryIOSurf(&m_mfxVideoParam, &allocRequest);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		MSDK_CHECK_RESULT_P_RET(status, MFX_ERR_NONE);
		allocRequest.NumFrameSuggested = m_assist + allocRequest.NumFrameSuggested;
		allocRequest.NumFrameMin = allocRequest.NumFrameSuggested;
		allocRequest.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE;
		allocRequest.Type |= (m_bAllowD3D) ? MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET : MFX_MEMTYPE_SYSTEM_MEMORY;
		memcpy(&allocRequest.Info, &m_mfxVideoParam.mfx.FrameInfo, sizeof(mfxFrameInfo));
		allocRequest.Info.Width = (mfxU16)MSDK_ALIGN32(allocRequest.Info.Width);
		allocRequest.Info.Height = (mfxU16)MSDK_ALIGN32(allocRequest.Info.Height);
		status = m_allocator->Alloc(m_allocator->pthis, &allocRequest, &m_mfxResponse);
		MSDK_CHECK_RESULT_P_RET(status, MFX_ERR_NONE);
		ASSERT(m_mfxResponse.NumFrameActual == allocRequest.NumFrameSuggested);
		m_mfxSurfaces.Reset(new mfxFrameSurface1[m_mfxResponse.NumFrameActual]);
		MSDK_CHECK_POINTER(m_mfxSurfaces, MFX_ERR_MEMORY_ALLOC);
		MSDK_ZERO_MEMORY(m_mfxSurfaces, sizeof(mfxFrameSurface1) * m_mfxResponse.NumFrameActual);
		for (mfxU32 i = 0; i < m_mfxResponse.NumFrameActual; ++i)
		{
			memcpy(&(m_mfxSurfaces[i].Info), &m_mfxVideoParam.mfx.FrameInfo, sizeof(mfxFrameInfo));
			m_mfxSurfaces[i].Data.MemId = m_mfxResponse.mids[i];
		}
		return status;
	}
	void QuickSyncDecoder::FreeFrames()
	{
		if (m_allocator != NULL)
		{
			m_allocator->Free(m_allocator->pthis, &m_mfxResponse);
		}
		MSDK_ZERO_MEMORY(&m_mfxResponse, sizeof(m_mfxResponse));
		m_mfxSurfaces.Reset(NULL);
	}
	void QuickSyncDecoder::DestoryAllocator()
	{
		if (m_allocator != NULL)
		{
			m_allocator->Close();
		}
		m_allocator.Reset(NULL);
		if (m_qsvd3d != NULL)
		{
			m_qsvd3d->Close();
		}
		m_qsvd3d.Reset(NULL);
	}

	BOOL QuickSyncDecoder::Open(BYTE* bits, LONG size)
	{
		m_mfxVersion.Major = MIN_REQUIRED_API_VER_MAJOR;
		m_mfxVersion.Minor = MIN_REQUIRED_API_VER_MINOR;
		mfxStatus status = MFX_ERR_UNSUPPORTED;
		INT adapter = GetIntelAdapter(NULL);
		if (adapter < 0)
		{
			status = MFX_ERR_UNSUPPORTED;
			goto _ERROR;
		}
		m_mfxIMPL |= MFX_IMPL_VIA_D3D9 | MFX_IMPL_AUTO_ANY;
#if MFX_D3D11_SUPPORT
		m_mfxIMPL |= MFX_IMPL_VIA_D3D11;
#endif
		m_mfxVideoSession.Reset(new MFXVideoSession());
		if (!m_mfxVideoSession)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		status = m_mfxVideoSession->Init(m_mfxIMPL, &m_mfxVersion);
		if (MSDK_FAILED(status))
			goto _ERROR;
		m_mfxVideoSession->QueryIMPL(&m_mfxIMPL);
		m_mfxVideoSession->QueryVersion(&m_mfxVersion);
		m_bHDW = m_mfxIMPL != MFX_IMPL_SOFTWARE;
		m_bAllowD3D = m_bHDW;
		m_bAllowD3D11 = m_bAllowD3D && ((m_mfxIMPL & MFX_IMPL_VIA_D3D11) == MFX_IMPL_VIA_D3D11);
		m_mfxVideoDECODE.Reset(new MFXVideoDECODE((mfxSession)*m_mfxVideoSession));
		if (!m_mfxVideoDECODE)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_streamBits[0].Reset(new BYTE[MAX_STREAM_SIZE]);
		if (NULL == m_streamBits[0])
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_streamBits[1].Reset(new BYTE[MAX_STREAM_SIZE]);
		if (NULL == m_streamBits[1])
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		memset(&m_mfxResidial, 0, sizeof(m_mfxResidial));
		m_mfxResidial.MaxLength = MAX_STREAM_SIZE;
		m_mfxResidial.Data = m_streamBits[1];
		if (MSDK_FAILED(status))
			goto _ERROR;
		status = InitializeVideoParam(bits, size);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = CreateAllocator();
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = AllocFrames();
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoDECODE->Init(&m_mfxVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoDECODE->GetVideoParam(&m_mfxVideoParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		return status == MFX_ERR_NONE;
	_ERROR:
		Close();
		return FALSE;
	}
	void QuickSyncDecoder::Close()
	{
		MSDK_ZERO_MEMORY(&m_mfxVideoParam, sizeof(m_mfxVideoParam));
		if (m_mfxVideoDECODE != NULL)
		{
			m_mfxVideoDECODE->Close();
		}
		m_mfxVideoDECODE.Reset(NULL);
		FreeFrames();
		if (m_mfxVideoSession != NULL)
		{
			if (m_bAllowD3D)
			{
				if (m_bAllowD3D11)
				{
#if MFX_D3D11_SUPPORT	
					m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D11_DEVICE, NULL);
#endif
				}
				else
				{
					m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, NULL);
				}
			}
			m_mfxVideoSession->Close();
		}
		m_mfxVideoSession.Reset(NULL);
		DestoryAllocator();
		m_bAllowD3D11 = FALSE;
		m_bAllowD3D = FALSE;
		m_bHDW = FALSE;
	}
	QSVAllocator* QuickSyncDecoder::GetAllocator()
	{
		return m_allocator;
	}
	BOOL QuickSyncDecoder::Decode(SampleTag& tag, mfxFrameSurface1*& video)
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_MEMORY_ALLOC);
		if (tag.size > 0)
		{
			m_tags.InsertLast(tag);
		}
		LONG residial = 0;
		mfxBitstream mfxStream;
		for (;;)
		{
			if (residial <= 0)
			{
				if (m_tags.GetSize() <= 0)
				{
					goto _DATA;
				}
				ITERATOR s = m_tags.First();
				SampleTag& sampleTag = m_tags.GetAt(s);
				m_tags.RemoveAt(s);
				ZeroMemory(&mfxStream, sizeof(mfxStream));
				mfxStream.TimeStamp = sampleTag.pts;
				INT32 newsize = sampleTag.size + m_mfxResidial.DataLength;
				mfxStream.MaxLength = mfxStream.DataLength = (mfxU32)newsize;
				mfxStream.Data = m_streamBits[0];
				mfxU8* pData = mfxStream.Data;
				memcpy(pData, m_mfxResidial.Data, m_mfxResidial.DataLength);
				pData += m_mfxResidial.DataLength;
				m_mfxResidial.DataLength = 0;
				memcpy(pData, sampleTag.bits, sampleTag.size);
				pData += sampleTag.size;
				residial += newsize;
				SAFE_DELETE_ARRAY(sampleTag.bits);
				mfxStatus status = Process(mfxStream, video);
				residial -= mfxStream.DataOffset;
				if (status != MFX_ERR_NONE)
				{
					if (status == MFX_ERR_MORE_DATA && residial == 0)
					{
						goto _DATA;
					}
					return FALSE;
				}
				else
				{
					m_outputs.InsertLast(video);
					if (residial == 0)
					{
						goto _DATA;
					}
				}
			}
			else
			{
				mfxStatus status = Process(mfxStream, video);
				residial -= mfxStream.DataOffset;
				if (status != MFX_ERR_NONE)
				{
					if (status == MFX_ERR_MORE_DATA && residial == 0)
					{
						goto _DATA;
					}
					goto _DATA;
				}
				else
				{
					m_outputs.InsertLast(video);
					if (residial == 0)
					{
						goto _DATA;
					}
				}
			}
		}
	_DATA:
		if (m_outputs.GetSize() > 0)
		{
			ITERATOR s = m_outputs.First();
			mfxFrameSurface1*& surface = m_outputs.GetAt(s);
			m_outputs.RemoveAt(s);
			tag.pts = tag.dts = surface->Data.TimeStamp;
			video = surface;
			return TRUE;
		}
		return FALSE;
	}
	void QuickSyncDecoder::LockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = pSurface - m_mfxSurfaces;
			if (index >= 0 && index < m_mfxResponse.NumFrameActual)
			{
				InterlockedIncrement(&m_locks[index]);
			}
		}
	}
	void QuickSyncDecoder::UnlockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = pSurface - m_mfxSurfaces;
			if (index >= 0 && index < m_mfxResponse.NumFrameActual)
			{
				ASSERT(m_locks[index] > 0);
				InterlockedDecrement(&m_locks[index]);
			}
		}
	}
	BOOL QuickSyncDecoder::IsSurfaceLocked(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = pSurface - m_mfxSurfaces;
			if (index >= 0 && index < m_mfxResponse.NumFrameActual)
			{
				return  (m_locks[index] > 0 || pSurface->Data.Locked > 0);
			}
		}
		return TRUE;
	}

	mfxFrameSurface1* QuickSyncDecoder::FindFreeSurface()
	{
		for (INT tries = 0; tries < 1000; ++tries)
		{
			for (int i = 0; i < m_mfxResponse.NumFrameActual; ++i)
			{
				if (!IsSurfaceLocked(m_mfxSurfaces + i))
				{
					return m_mfxSurfaces + i;
				}
			}
			Sleep(1);
		}
		return NULL;
	}
	mfxStatus QuickSyncDecoder::Process(mfxBitstream& stream, mfxFrameSurface1*& video)
	{
		video = NULL;
		mfxStatus status = MFX_ERR_NONE;
		mfxSyncPoint syncpDECODE = NULL;
		while (stream.DataLength > 0)
		{
			mfxFrameSurface1*surface = FindFreeSurface();
			MSDK_CHECK_POINTER(surface, MFX_ERR_NOT_ENOUGH_BUFFER);
			do
			{
				status = m_mfxVideoDECODE->DecodeFrameAsync(&stream, surface, &video, &syncpDECODE);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					surface = FindFreeSurface();
					MSDK_CHECK_POINTER(surface, MFX_ERR_NOT_ENOUGH_BUFFER);
				}
				if (MFX_WRN_DEVICE_BUSY == status)
				{
					Sleep(1);
				}
			} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
			if (MFX_ERR_NONE < status && syncpDECODE)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_mfxVideoSession->SyncOperation(syncpDECODE, 0xFFFF);
				if (MFX_ERR_NONE == status)
				{
					LockSurface(video);
				}
				return status;
			}
		}
		return status;
	}
}

