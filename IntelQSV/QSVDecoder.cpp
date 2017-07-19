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
	BOOL QSVDecoder::Decode(Media::SampleTag& tag, mfxFrameSurface1*& video)
	{
		mfxBitstream stream;
		memset(&stream, 0, sizeof(stream));
		stream.TimeStamp = tag.samplePTS;
		INT32 newsize = tag.size + m_mfxResidial.DataLength;
		stream.MaxLength = stream.DataLength = (mfxU32)newsize;
		stream.Data = m_streamBits[0];
		mfxU8* pData = stream.Data;
		memcpy(pData, m_mfxResidial.Data, m_mfxResidial.DataLength);
		pData += m_mfxResidial.DataLength;
		m_mfxResidial.DataLength = 0;
		memcpy(pData, tag.bits, tag.size);
		pData += tag.size;
		mfxStatus status = Process(stream, tag, video);
		if (status != MFX_ERR_NONE)
			return FALSE;
		m_mfxResidial.DataOffset = 0;
		m_mfxResidial.DataLength = stream.DataLength;
		m_mfxResidial.TimeStamp = tag.samplePTS;
		if (stream.DataLength > m_mfxResidial.MaxLength)
		{
			m_streamBits[1].Reset(new mfxU8[m_mfxResidial.MaxLength]);
			m_mfxResidial.MaxLength = stream.DataLength;
			m_mfxResidial.Data = m_streamBits[1];
		}
		memcpy(m_mfxResidial.Data, stream.Data + stream.DataOffset, stream.DataLength);
		return TRUE;
	}
	mfxStatus QSVDecoder::Process(mfxBitstream& stream, Media::SampleTag& tag, mfxFrameSurface1*& video)
	{
		mfxStatus status = MFX_ERR_NONE;
		while (stream.DataLength > 0)
		{
			mfxFrameSurface1* surface = NULL;
			mfxU16 index = GetFreeSurfaceIndex(m_mfxSurfaces, m_mfxResponse.NumFrameActual);
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
			do
			{
				status = m_mfxVideoDECODE->DecodeFrameAsync(&stream, m_mfxSurfaces[index], &surface, &m_syncpDECODE);
				if (MFX_ERR_MORE_SURFACE == status)
				{
					index = GetFreeSurfaceIndex(m_mfxSurfaces, m_mfxResponse.NumFrameActual);
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
				}
				if (MFX_WRN_DEVICE_BUSY == status)
				{
					Sleep(1);
				}
			} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
			if (MFX_ERR_NONE < status && m_syncpDECODE)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				do
				{
					status = m_mfxSession.SyncOperation(m_syncpDECODE, 1000);
				} while (status == MFX_WRN_IN_EXECUTION);
				tag.samplePTS = surface->Data.TimeStamp;
				tag.sampleDTS = surface->Data.TimeStamp;
				if (MFX_ERR_NONE == status)
				{
					mfxU16 index1 = GetFreeSurfaceIndex(m_mfxVPPSurfaces, m_mfxVppResponse.NumFrameActual);
					MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
					do
					{
						status = m_mfxVideoVPP->RunFrameVPPAsync(surface, m_mfxVPPSurfaces[index1], NULL, &m_syncpVPP);
						if (MFX_ERR_MORE_SURFACE == status)
						{
							index1 = GetFreeSurfaceIndex(m_mfxVPPSurfaces, m_mfxVppResponse.NumFrameActual);
							MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
						}
						if (MFX_WRN_DEVICE_BUSY == status)
						{
							Sleep(1);
						}
					} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
					if (MFX_ERR_NONE < status && m_syncpVPP)
					{
						status = MFX_ERR_NONE;
					}
					if (MFX_ERR_NONE == status)
					{
						do
						{
							status = m_mfxSession.SyncOperation(m_syncpVPP, 1000);
						} while (status == MFX_WRN_IN_EXECUTION);
						if (status == MFX_ERR_NONE)
						{
							video = m_mfxVPPSurfaces[index1];
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
		if (m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN == 0 || !m_mfxVideoParam.mfx.FrameInfo.FrameRateExtD)
		{
			m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN = 30;
			m_mfxVideoParam.mfx.FrameInfo.FrameRateExtD = 1;
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
		m_allocator.Reset(NULL);
		m_qsvd3d.Reset(NULL);
	}
	mfxStatus QSVDecoder::AllocFrames()
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		mfxFrameAllocRequest request;
		mfxFrameAllocRequest requestVPP[2];
		mfxU16 nSurfNum = 0;
		mfxU16 nVppSurfNum = 0;
		MSDK_ZERO_MEMORY(request);
		MSDK_ZERO_MEMORY(requestVPP[0]);
		MSDK_ZERO_MEMORY(requestVPP[1]);
		//status = m_mfxVideoDECODE->Query(&m_mfxVideoParam, &m_mfxVideoParam);
		//MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		//if (MFX_ERR_NONE != status)
		//	goto _ERROR;
		status = m_mfxVideoDECODE->QueryIOSurf(&m_mfxVideoParam, &request);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		/*m_mfxVppVideoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
		MSDK_MEMCPY_VAR(m_mfxVppVideoParam.vpp.In, &m_mfxVideoParam.mfx.FrameInfo, sizeof(mfxFrameInfo));
		MSDK_MEMCPY_VAR(m_mfxVppVideoParam.vpp.Out, &m_mfxVppVideoParam.vpp.In, sizeof(mfxFrameInfo));
		m_mfxVppVideoParam.vpp.Out.FourCC = MFX_FOURCC_RGB4;*/
		memset(&m_mfxVppVideoParam, 0, sizeof(m_mfxVppVideoParam));
		m_mfxVppVideoParam.vpp.In.FourCC = MFX_FOURCC_NV12;
		m_mfxVppVideoParam.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_mfxVppVideoParam.vpp.In.CropX = 0;
		m_mfxVppVideoParam.vpp.In.CropY = 0;
		m_mfxVppVideoParam.vpp.In.CropW = m_mfxVideoParam.mfx.FrameInfo.Width;
		m_mfxVppVideoParam.vpp.In.CropH = m_mfxVideoParam.mfx.FrameInfo.Height;
		m_mfxVppVideoParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxVppVideoParam.vpp.In.FrameRateExtN = 30;
		m_mfxVppVideoParam.vpp.In.FrameRateExtD = 1;
		m_mfxVppVideoParam.vpp.In.Width = MSDK_ALIGN16(m_mfxVppVideoParam.vpp.In.CropW);
		m_mfxVppVideoParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppVideoParam.vpp.In.PicStruct) ? MSDK_ALIGN16(m_mfxVppVideoParam.vpp.In.CropH) : MSDK_ALIGN32(m_mfxVppVideoParam.vpp.In.CropH);
		m_mfxVppVideoParam.vpp.Out.FourCC = MFX_FOURCC_RGB4;
		m_mfxVppVideoParam.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_mfxVppVideoParam.vpp.Out.CropX = 0;
		m_mfxVppVideoParam.vpp.Out.CropY = 0;
		m_mfxVppVideoParam.vpp.Out.CropW = m_mfxVppVideoParam.vpp.In.CropW;
		m_mfxVppVideoParam.vpp.Out.CropH = m_mfxVppVideoParam.vpp.In.CropH;
		m_mfxVppVideoParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxVppVideoParam.vpp.Out.FrameRateExtN = 30;
		m_mfxVppVideoParam.vpp.Out.FrameRateExtD = 1;
		m_mfxVppVideoParam.vpp.Out.Width = MSDK_ALIGN16(m_mfxVppVideoParam.vpp.Out.CropW);
		m_mfxVppVideoParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppVideoParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(m_mfxVppVideoParam.vpp.Out.CropH) : MSDK_ALIGN32(m_mfxVppVideoParam.vpp.Out.CropH);
		m_mfxVppVideoParam.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

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
		m_mfxVppVideoParam.ExtParam = &m_vppExtParams[0];
		m_mfxVppVideoParam.NumExtParam = (mfxU16)m_vppExtParams.size();
		/*status = m_mfxVideoVPP->Query(&m_mfxVppVideoParam, &m_mfxVppVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		if (MFX_ERR_NONE != status)
			return status;*/
		status = m_mfxVideoVPP->QueryIOSurf(&m_mfxVppVideoParam, requestVPP);
		if (MFX_WRN_PARTIAL_ACCELERATION == status)
		{
			MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		}
		nSurfNum = request.NumFrameSuggested + requestVPP[0].NumFrameSuggested;
		nVppSurfNum = requestVPP[1].NumFrameSuggested;
		request.NumFrameSuggested = request.NumFrameMin = nSurfNum;
		request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
		status = m_allocator->Alloc(m_allocator->pthis, &request, &m_mfxResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		MSDK_MEMCPY_VAR(requestVPP[1].Info, &(m_mfxVppVideoParam.vpp.Out), sizeof(mfxFrameInfo));
		status = m_allocator->Alloc(m_allocator->pthis, &requestVPP[1], &m_mfxVppResponse);
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
		nVppSurfNum = m_mfxVppResponse.NumFrameActual;
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
			m_mfxVPPSurfaces[i]->Data.MemId = m_mfxVppResponse.mids[i];
		}
	_ERROR:
		return status;
	}
	void QSVDecoder::DeleteFrames()
	{
		if (m_allocator != NULL)
		{
			m_allocator->Free(m_allocator->pthis, &m_mfxResponse);
			m_allocator->Free(m_allocator->pthis, &m_mfxVppResponse);
		}
		for (INT i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxSurfaces[i]);
		}
		m_mfxSurfaces.Reset(NULL);
		for (INT i = 0; i < m_mfxVppResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxVPPSurfaces[i]);
		}
		m_mfxVPPSurfaces.Reset(NULL);
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
}
