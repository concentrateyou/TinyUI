#include "stdafx.h"
#include "QSVDecoder.h"

namespace QSV
{
	QSVDecoder::QSVDecoder()
	{
		MSDK_ZERO_MEMORY(m_mfxVideoParam);
		MSDK_ZERO_MEMORY(m_mfxVppVideoParam);
		MSDK_ZERO_MEMORY(m_vppDoNotUse);
		m_vppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
		m_vppDoNotUse.Header.BufferSz = sizeof(m_vppDoNotUse);
	}

	QSVDecoder::~QSVDecoder()
	{
		Close();
	}
	BOOL QSVDecoder::Open(const BYTE* bits, LONG size)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxInitParam initParam = { 0 };
		initParam.Version.Major = 1;
		initParam.Version.Minor = 0;
		initParam.Implementation |= MFX_IMPL_HARDWARE_ANY | MFX_IMPL_VIA_D3D9;
		status = m_mfxSession.InitEx(initParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxSession.QueryVersion(&m_mfxVersion);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxSession.QueryIMPL(&m_mfxImpl);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		m_mfxVideoDECODE.Reset(new MFXVideoDECODE(m_mfxSession));
		if (NULL == m_mfxVideoDECODE)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_mfxVideoVPP.Reset(new MFXVideoVPP(m_mfxSession));
		if (NULL == m_mfxVideoVPP)
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
		status = m_mfxVideoVPP->Init(&m_mfxVppVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoDECODE->GetVideoParam(&m_mfxVideoParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
	_ERROR:
		return status == MFX_ERR_NONE;
	}
	BOOL QSVDecoder::Decode(SampleTag& tag, mfxFrameSurface1*& video)
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
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
				ITERATOR s = m_tags.First();
				Media::SampleTag& sampleTag = m_tags.GetAt(s);
				m_tags.RemoveAt(s);
				memset(&mfxStream, 0, sizeof(mfxStream));
				mfxStream.TimeStamp = sampleTag.samplePTS;
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
				mfxStatus status = Process(mfxStream, video);
				residial -= mfxStream.DataOffset;
				if (status != MFX_ERR_NONE)
				{
					if (status == MFX_ERR_MORE_DATA && residial == 0)
					{
						goto _DATA;
					}
					SAFE_DELETE_ARRAY(tag.bits);
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
					SAFE_DELETE_ARRAY(tag.bits);
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
		SAFE_DELETE_ARRAY(tag.bits);
		if (m_outputs.GetSize() > 0)
		{
			ITERATOR s = m_outputs.First();
			mfxFrameSurface1*& surface = m_outputs.GetAt(s);
			m_outputs.RemoveAt(s);
			tag.samplePTS = tag.sampleDTS = surface->Data.TimeStamp;
			video = surface;
			return TRUE;
		}
		return FALSE;
	}
	INT QSVDecoder::GetFreeVPPSurfaceIndex()
	{
		for (INT i = 0; i < m_mfxVPPResponse.NumFrameActual; ++i)
		{
			BOOL locked = m_locks[i] > 0 || (m_mfxVPPSurfaces[i]->Data.Locked > 0);
			if (!locked)
			{
				return i;
			}
		}
		return MFX_ERR_NOT_FOUND;
	}
	void QSVDecoder::LockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = -1;
			for (INT i = 0;i < m_mfxVPPResponse.NumFrameActual;i++)
			{
				if (m_mfxVPPSurfaces[i] == pSurface)
				{
					index = i;
					break;
				}
			}
			if (index >= 0 && index < m_mfxVPPResponse.NumFrameActual)
			{
				InterlockedIncrement(&m_locks[index]);
			}
		}
	}

	void QSVDecoder::UnlockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = -1;
			for (INT i = 0;i < m_mfxVPPResponse.NumFrameActual;i++)
			{
				if (m_mfxVPPSurfaces[i] == pSurface)
				{
					index = i;
					break;
				}
			}
			if (index >= 0 && index < m_mfxVPPResponse.NumFrameActual)
			{
				ASSERT(m_locks[index] > 0);
				InterlockedDecrement(&m_locks[index]);
			}
		}
	}
	INT QSVDecoder::GetFreeVideoSurfaceIndex()
	{
		for (INT i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			if (0 == m_mfxSurfaces[i]->Data.Locked)
				return i;
		}
		return MFX_ERR_NOT_FOUND;
	}
	mfxStatus QSVDecoder::Process(mfxBitstream& stream, mfxFrameSurface1*& video)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxSyncPoint syncpDECODE = NULL;
		mfxSyncPoint syncpVPP = NULL;
		while (stream.DataLength > 0)
		{
			mfxFrameSurface1* surface = NULL;
			INT index = GetFreeVideoSurfaceIndex();
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_mfxVideoDECODE->DecodeFrameAsync(&stream, m_mfxSurfaces[index], &surface, &syncpDECODE);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeVideoSurfaceIndex();
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
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
				do
				{
					status = m_mfxSession.SyncOperation(syncpDECODE, 1000);
				} while (status == MFX_WRN_IN_EXECUTION);
				if (MFX_ERR_NONE == status)
				{
					INT index1 = GetFreeVPPSurfaceIndex();
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
					do
					{
						status = m_mfxVideoVPP->RunFrameVPPAsync(surface, m_mfxVPPSurfaces[index1], NULL, &syncpVPP);
						if (MFX_ERR_MORE_SURFACE == status)
						{
							index1 = GetFreeVPPSurfaceIndex();
							MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
						}
						if (MFX_WRN_DEVICE_BUSY == status)
						{
							Sleep(1);
						}
					} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
					if (MFX_ERR_NONE < status && syncpVPP)
					{
						status = MFX_ERR_NONE;
					}
					if (MFX_ERR_NONE == status)
					{
						do
						{
							status = m_mfxSession.SyncOperation(syncpVPP, 1000);
						} while (status == MFX_WRN_IN_EXECUTION);
						if (status == MFX_ERR_NONE)
						{
							video = m_mfxVPPSurfaces[index1];
							LockSurface(video);
						}
						return status;
					}
				}
			}
		}
		return status;
	}
	mfxStatus QSVDecoder::InitializeVideoParam(const BYTE* bits, LONG size)
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		memset(&m_mfxVideoParam, 0, sizeof(m_mfxVideoParam));
		m_mfxVideoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_mfxVideoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
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
		if (m_mfxVideoParam.AsyncDepth == 0)
		{
			m_mfxVideoParam.AsyncDepth = 4;
		}
		return status;
	}
	mfxStatus QSVDecoder::CreateAllocator()
	{
		mfxStatus status = MFX_ERR_NONE;
		HWND hWND = NULL;
		mfxHDL handle = NULL;
		POINT pos = { 0 };
		QSVD3D9AllocatorParams* pParams = NULL;
		m_allocator.Reset(new GeneralAllocator());
		if (NULL == m_allocator)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_qsvd3d.Reset(new QSVD3D9());
		if (NULL == m_qsvd3d)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}

		hWND = ::WindowFromPoint(pos);
		if (!hWND)
		{
			hWND = GetDesktopWindow();
		}
		status = m_qsvd3d->Initialize(hWND, GetIntelAdapter(m_mfxSession));
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		handle = m_qsvd3d->GetHandle(MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9);
		if (NULL == handle)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		status = m_mfxSession.SetHandle(MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9, handle);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		pParams = new QSVD3D9AllocatorParams();
		if (NULL == pParams)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		pParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(handle);
		status = m_allocator->Initialize(pParams);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxSession.SetFrameAllocator(m_allocator);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
	_ERROR:
		SAFE_DELETE(pParams);
		return status;
	}
	void QSVDecoder::DeleteAllocator()
	{
		if (m_allocator)
			m_allocator->Close();
		m_allocator.Reset(NULL);
		if (m_qsvd3d)
			m_qsvd3d->Close();
		m_qsvd3d.Reset(NULL);
	}
	mfxStatus QSVDecoder::AllocFrames()
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		mfxFrameAllocRequest request;
		mfxFrameAllocRequest requestVPP[2];
		mfxU16 nSurfNum = 0;
		mfxU16 nVppSurfNum = 0;
		MSDK_ZERO_MEMORY(request);
		MSDK_ZERO_MEMORY(requestVPP[0]);
		MSDK_ZERO_MEMORY(requestVPP[1]);
		status = m_mfxVideoDECODE->QueryIOSurf(&m_mfxVideoParam, &request);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		MSDK_MEMCPY_VAR(m_mfxVppVideoParam.vpp.In, &m_mfxVideoParam.mfx.FrameInfo, sizeof(mfxFrameInfo));
		MSDK_MEMCPY_VAR(m_mfxVppVideoParam.vpp.Out, &m_mfxVppVideoParam.vpp.In, sizeof(mfxFrameInfo));
		m_mfxVppVideoParam.vpp.Out.FourCC = MFX_FOURCC_RGB4;
		m_mfxVppVideoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
		m_vppDoNotUse.NumAlg = 4;
		m_vppDoNotUse.AlgList = new mfxU32[m_vppDoNotUse.NumAlg];
		if (NULL == m_vppDoNotUse.AlgList)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_vppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		m_vppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		m_vppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		m_vppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;
		m_vppExtParams.push_back((mfxExtBuffer*)&m_vppDoNotUse);
		m_mfxVppVideoParam.AsyncDepth = m_mfxVideoParam.AsyncDepth;
		m_mfxVppVideoParam.ExtParam = &m_vppExtParams[0];
		m_mfxVppVideoParam.NumExtParam = (mfxU16)m_vppExtParams.size();
		status = m_mfxVideoVPP->QueryIOSurf(&m_mfxVppVideoParam, requestVPP);
		if (MFX_WRN_PARTIAL_ACCELERATION == status)
		{
			MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		}
		if ((requestVPP[0].NumFrameSuggested < m_mfxVppVideoParam.AsyncDepth) ||
			(requestVPP[1].NumFrameSuggested < m_mfxVppVideoParam.AsyncDepth))
			return MFX_ERR_MEMORY_ALLOC;
		nSurfNum = request.NumFrameSuggested + requestVPP[0].NumFrameSuggested - m_mfxVideoParam.AsyncDepth + 1;
		nVppSurfNum = requestVPP[1].NumFrameSuggested + 1;
		request.NumFrameSuggested = request.NumFrameMin = nSurfNum;
		request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
		status = m_allocator->Alloc(m_allocator->pthis, &request, &m_mfxResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		MSDK_MEMCPY_VAR(requestVPP[1].Info, &(m_mfxVppVideoParam.vpp.Out), sizeof(mfxFrameInfo));
		requestVPP[1].NumFrameSuggested = requestVPP[1].NumFrameMin = nVppSurfNum;
		status = m_allocator->Alloc(m_allocator->pthis, &requestVPP[1], &m_mfxVPPResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		nSurfNum = m_mfxResponse.NumFrameActual;
		m_mfxSurfaces.Reset(new mfxFrameSurface1*[nSurfNum]);
		if (NULL == m_mfxSurfaces)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		for (INT i = 0; i < nSurfNum; i++)
		{
			m_mfxSurfaces[i] = new mfxFrameSurface1();
			MSDK_MEMCPY_VAR(m_mfxSurfaces[i]->Info, &(request.Info), sizeof(mfxFrameInfo));
			m_mfxSurfaces[i]->Data.MemId = m_mfxResponse.mids[i];
		}
		nVppSurfNum = m_mfxVPPResponse.NumFrameActual;
		m_mfxVPPSurfaces.Reset(new mfxFrameSurface1*[nVppSurfNum]);
		if (NULL == m_mfxVPPSurfaces)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		for (INT i = 0; i < nVppSurfNum; i++)
		{
			m_mfxVPPSurfaces[i] = new mfxFrameSurface1();
			MSDK_MEMCPY_VAR(m_mfxVPPSurfaces[i]->Info, &requestVPP[1].Info, sizeof(mfxFrameInfo));
			m_mfxVPPSurfaces[i]->Data.MemId = m_mfxVPPResponse.mids[i];
		}
	_ERROR:
		return status;
	}
	void QSVDecoder::DeleteFrames()
	{
		if (m_allocator != NULL)
		{
			m_allocator->Free(m_allocator->pthis, &m_mfxResponse);
			m_allocator->Free(m_allocator->pthis, &m_mfxVPPResponse);
		}
		for (mfxU16 i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxSurfaces[i]);
		}
		m_mfxSurfaces.Reset(NULL);
		for (mfxU16 i = 0; i < m_mfxVPPResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxVPPSurfaces[i]);
		}
		m_mfxVPPSurfaces.Reset(NULL);
		ZeroMemory(&m_mfxResponse, sizeof(m_mfxResponse));
		ZeroMemory(&m_mfxVPPResponse, sizeof(m_mfxVPPResponse));
	}
	void QSVDecoder::Close()
	{
		if (m_mfxVideoVPP)
			m_mfxVideoVPP->Close();
		m_mfxVideoVPP.Reset(NULL);
		if (m_mfxVideoDECODE)
			m_mfxVideoDECODE->Close();
		m_mfxVideoDECODE.Reset(NULL);
		DeleteFrames();
		m_mfxSession.Close();
		MSDK_SAFE_DELETE_ARRAY(m_vppDoNotUse.AlgList);
		DeleteAllocator();
	}
	QSVAllocator* QSVDecoder::GetAllocator()
	{
		return m_allocator;
	}
}
