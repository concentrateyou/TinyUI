#include "stdafx.h"
#include "QSVEncoder.h"

namespace QSV
{
	PartiallyLinearFNC::PartiallyLinearFNC()
		: m_pX()
		, m_pY()
		, m_nPoints()
		, m_nAllocated()
	{
	}
	PartiallyLinearFNC::~PartiallyLinearFNC()
	{
		delete[]m_pX;
		m_pX = NULL;
		delete[]m_pY;
		m_pY = NULL;
	}
	void PartiallyLinearFNC::AddPair(mfxF64 x, mfxF64 y)
	{
		for (mfxU32 i = 0; i < m_nPoints; i++)
		{
			if (m_pX[i] == x)
				return;
		}
		if (m_nPoints == m_nAllocated)
		{
			m_nAllocated += 20;
			mfxF64 * pnew;
			pnew = new mfxF64[m_nAllocated];
			MSDK_MEMCPY_BUF(pnew, 0, sizeof(mfxF64)*m_nAllocated, m_pX, sizeof(mfxF64) * m_nPoints);
			delete[] m_pX;
			m_pX = pnew;

			pnew = new mfxF64[m_nAllocated];
			MSDK_MEMCPY_BUF(pnew, 0, sizeof(mfxF64)*m_nAllocated, m_pY, sizeof(mfxF64) * m_nPoints);
			delete[] m_pY;
			m_pY = pnew;
		}
		m_pX[m_nPoints] = x;
		m_pY[m_nPoints] = y;

		m_nPoints++;
	}
	mfxF64 PartiallyLinearFNC::GetAt(mfxF64 x)
	{
		if (m_nPoints < 2)
		{
			return 0;
		}
		bool bwasmin = false;
		bool bwasmax = false;

		mfxU32 maxx = 0;
		mfxU32 minx = 0;
		mfxU32 i;

		for (i = 0; i < m_nPoints; i++)
		{
			if (m_pX[i] <= x && (!bwasmin || m_pX[i] > m_pX[maxx]))
			{
				maxx = i;
				bwasmin = true;
			}
			if (m_pX[i] > x && (!bwasmax || m_pX[i] < m_pX[minx]))
			{
				minx = i;
				bwasmax = true;
			}
		}
		if (!bwasmin)
		{
			for (i = 0; i < m_nPoints; i++)
			{
				if (m_pX[i] > m_pX[minx] && (!bwasmin || m_pX[i] < m_pX[minx]))
				{
					maxx = i;
					bwasmin = true;
				}
			}
		}
		if (!bwasmax)
		{
			for (i = 0; i < m_nPoints; i++)
			{
				if (m_pX[i] < m_pX[maxx] && (!bwasmax || m_pX[i] > m_pX[minx]))
				{
					minx = i;
					bwasmax = true;
				}
			}
		}
		return (x - m_pX[minx])*(m_pY[maxx] - m_pY[minx]) / (m_pX[maxx] - m_pX[minx]) + m_pY[minx];
	}
	//////////////////////////////////////////////////////////////////////////
	mfxU16 CalculateDefaultBitrate(mfxU32 nCodecId, mfxU32 nTargetUsage, mfxU32 nWidth, mfxU32 nHeight, mfxF64 dFrameRate)
	{
		PartiallyLinearFNC fnc;
		mfxF64 bitrate = 0;

		switch (nCodecId)
		{
		case MFX_CODEC_AVC:
		{
			fnc.AddPair(0, 0);
			fnc.AddPair(25344, 225);
			fnc.AddPair(101376, 1000);
			fnc.AddPair(414720, 4000);
			fnc.AddPair(2058240, 5000);
			break;
		}
		case MFX_CODEC_MPEG2:
		{
			fnc.AddPair(0, 0);
			fnc.AddPair(414720, 12000);
			break;
		}
		default:
		{
			fnc.AddPair(0, 0);
			fnc.AddPair(414720, 12000);
			break;
		}
		}

		mfxF64 at = nWidth * nHeight * dFrameRate / 30.0;

		if (!at)
			return 0;

		switch (nTargetUsage)
		{
		case MFX_TARGETUSAGE_BEST_QUALITY:
		{
			bitrate = (&fnc)->GetAt(at);
			break;
		}
		case MFX_TARGETUSAGE_BEST_SPEED:
		{
			bitrate = (&fnc)->GetAt(at) * 0.5;
			break;
		}
		case MFX_TARGETUSAGE_BALANCED:
		default:
		{
			bitrate = (&fnc)->GetAt(at) * 0.75;
			break;
		}
		}

		return (mfxU16)bitrate;
	}
	mfxU16 FourCCToChroma(mfxU32 fourCC)
	{
		switch (fourCC)
		{
		case MFX_FOURCC_NV12:
		case MFX_FOURCC_P010:
			return MFX_CHROMAFORMAT_YUV420;
		case MFX_FOURCC_NV16:
		case MFX_FOURCC_P210:
		case MFX_FOURCC_YUY2:
			return MFX_CHROMAFORMAT_YUV422;
		case MFX_FOURCC_RGB4:
			return MFX_CHROMAFORMAT_YUV444;
		}
		return MFX_CHROMAFORMAT_YUV420;
	}
	mfxStatus ConvertFrameRate(mfxF64 dFrameRate, mfxU32* pnFrameRateExtN, mfxU32* pnFrameRateExtD)
	{
		MSDK_CHECK_POINTER(pnFrameRateExtN, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(pnFrameRateExtD, MFX_ERR_NULL_PTR);
		mfxU32 fr;
		fr = (mfxU32)(dFrameRate + .5);

		if (fabs(fr - dFrameRate) < 0.0001)
		{
			*pnFrameRateExtN = fr;
			*pnFrameRateExtD = 1;
			return MFX_ERR_NONE;
		}
		fr = (mfxU32)(dFrameRate * 1.001 + .5);
		if (fabs(fr * 1000 - dFrameRate * 1001) < 10)
		{
			*pnFrameRateExtN = fr * 1000;
			*pnFrameRateExtD = 1001;
			return MFX_ERR_NONE;
		}
		*pnFrameRateExtN = (mfxU32)(dFrameRate * 10000 + .5);
		*pnFrameRateExtD = 10000;
		return MFX_ERR_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	QSVEncoder::QSVEncoder()
	{
		ZeroMemory(&m_mfxEncodeParam, sizeof(m_mfxEncodeParam));
		ZeroMemory(&m_mfxVppParam, sizeof(m_mfxVppParam));
	}
	QSVEncoder::~QSVEncoder()
	{
	}
	BOOL QSVEncoder::Open(const TinySize& src, const TinySize& dest)
	{
		mfxStatus status = MFX_ERR_NONE;
		m_mfxResidial.MaxLength = 1024 * 1024 * 5;
		m_mfxResidial.Data = new mfxU8[m_mfxResidial.MaxLength];
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
		m_mfxVideoENCODE.Reset(new MFXVideoENCODE(m_mfxSession));
		if (NULL == m_mfxVideoENCODE)
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
		status = InitializeParam(src, dest);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = CreateAllocator(src, dest);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = AllocFrames();
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoENCODE->Init(&m_mfxEncodeParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoVPP->Init(&m_mfxVppParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoENCODE->GetVideoParam(&m_mfxEncodeParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
	_ERROR:
		return status == MFX_ERR_NONE;
	}
	void QSVEncoder::Close()
	{
		if (m_mfxVideoENCODE != NULL)
		{
			m_mfxVideoENCODE->Close();
			m_mfxVideoENCODE.Reset(NULL);
		}
		if (m_mfxVideoVPP != NULL)
		{
			m_mfxVideoVPP->Close();
			m_mfxVideoVPP.Reset(NULL);
		}
		SAFE_DELETE_ARRAY(m_mfxResidial.Data);
		MSDK_SAFE_DELETE_ARRAY(m_vppDoNotUse.AlgList);
		DeleteFrames();
		DeleteAllocator();
	}
	BOOL QSVEncoder::GetSPSPPS(vector<mfxU8>& sps, vector<mfxU8>& pps)
	{
		if (!m_mfxVideoENCODE)
			return FALSE;
		mfxU8	mfxsps[100];
		mfxU8	mfxpps[100];
		mfxVideoParam par;
		ZeroMemory(&par, sizeof(par));
		mfxExtCodingOptionSPSPPS spspps;
		ZeroMemory(&spspps, sizeof(mfxExtCodingOptionSPSPPS));
		spspps.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
		spspps.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);
		spspps.PPSBuffer = mfxpps;
		spspps.PPSBufSize = 100;
		spspps.SPSBuffer = mfxsps;
		spspps.SPSBufSize = 100;
		mfxExtBuffer* ext[1];
		ext[0] = (mfxExtBuffer *)&spspps;
		par.ExtParam = &ext[0];
		par.NumExtParam = 1;
		if (m_mfxVideoENCODE->GetVideoParam(&par) == MFX_ERR_NONE)
		{
			sps.resize(spspps.SPSBufSize);
			memcpy(&sps[0], mfxsps, spspps.SPSBufSize);
			pps.resize(spspps.PPSBufSize);
			memcpy(&sps[0], mfxpps, spspps.PPSBufSize);
			return TRUE;
		}
		return FALSE;
	}
	void QSVEncoder::LockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = -1;
			for (INT i = 0;i < m_mfxResponse.NumFrameActual;i++)
			{
				if (m_mfxSurfaces[i] == pSurface)
				{
					index = i;
					break;
				}
			}
			if (index >= 0 && index < m_mfxResponse.NumFrameActual)
			{
				InterlockedIncrement(&m_locks[index]);
			}
		}
	}
	void QSVEncoder::UnlockSurface(mfxFrameSurface1* pSurface)
	{
		if (pSurface != NULL)
		{
			INT index = -1;
			for (INT i = 0;i < m_mfxResponse.NumFrameActual;i++)
			{
				if (m_mfxSurfaces[i] == pSurface)
				{
					index = i;
					break;
				}
			}
			if (index >= 0 && index < m_mfxResponse.NumFrameActual)
			{
				ASSERT(m_locks[index] > 0);
				InterlockedDecrement(&m_locks[index]);
			}
		}
	}
	BOOL QSVEncoder::Encode(SampleTag& tag)
	{
		MSDK_CHECK_POINTER(m_mfxVideoENCODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
		mfxStatus status = MFX_ERR_NONE;
		return Process(tag.bits, tag.size);
	}
	mfxStatus QSVEncoder::Process(const BYTE* bits, LONG size)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxSyncPoint syncpVPP = NULL;
		mfxSyncPoint syncpEncode = NULL;
		INT index = GetFreeVPPSurfaceIndex();
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
		INT index1 = GetFreeVideoSurfaceIndex();
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
		do
		{
			mfxFrameSurface1* surface = m_mfxVPPSurfaces[index];
			m_allocator->Lock(m_allocator->pthis, surface->Data.MemId, &(surface->Data));
			mfxU16 pitch = surface->Data.Pitch;
			mfxU8 *pRGB = MSDK_MIN(MSDK_MIN(surface->Data.R, surface->Data.G), surface->Data.B);
			pRGB = pRGB + surface->Info.CropX + surface->Info.CropY * surface->Data.Pitch;
			const BYTE* pData = bits;
			for (mfxU16 i = 0; i < surface->Info.Height; i++)
			{
				memcpy(pRGB + i * pitch, pData, 4 * surface->Info.Width);
				pData += 4 * surface->Info.Width;
			}
			m_allocator->Unlock(m_allocator->pthis, surface->Data.MemId, &(surface->Data));
			status = m_mfxVideoVPP->RunFrameVPPAsync(surface, m_mfxSurfaces[index1], NULL, &syncpVPP);
			if (MFX_ERR_MORE_SURFACE == status)
			{
				index = GetFreeVPPSurfaceIndex();
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
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
			if (MFX_ERR_NONE == status)
			{
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
				do
				{
					m_mfxEncodeCtrl.FrameType = MFX_FRAMETYPE_UNKNOWN;
					status = m_mfxVideoENCODE->EncodeFrameAsync(&m_mfxEncodeCtrl, m_mfxSurfaces[index1], &m_mfxResidial, &syncpEncode);
					if (MFX_ERR_MORE_SURFACE == status)
					{
						index1 = GetFreeVideoSurfaceIndex();
						MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
					}
					if (MFX_WRN_DEVICE_BUSY == status)
					{
						Sleep(1);
					}
				} while (MFX_WRN_DEVICE_BUSY == status || MFX_ERR_MORE_SURFACE == status);
				if (MFX_ERR_NONE < status && syncpEncode)
				{
					status = MFX_ERR_NONE;
				}
				if (MFX_ERR_NONE == status)
				{
					do
					{
						status = m_mfxSession.SyncOperation(syncpEncode, 1000);
					} while (status == MFX_WRN_IN_EXECUTION);
					return status;
				}
			}
		}
		return status;
	}
	INT QSVEncoder::GetFreeVPPSurfaceIndex()
	{
		for (INT i = 0; i < m_mfxVPPResponse.NumFrameActual; i++)
		{
			if (0 == m_mfxVPPSurfaces[i]->Data.Locked)
				return i;
		}
		return MFX_ERR_NOT_FOUND;
	}
	INT QSVEncoder::GetFreeVideoSurfaceIndex()
	{
		for (INT i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			if (0 == m_mfxSurfaces[i]->Data.Locked)
				return i;
		}
		return MFX_ERR_NOT_FOUND;
	}
	mfxStatus QSVEncoder::CreateAllocator(const TinySize& src, const TinySize& dest)
	{
		mfxStatus status = MFX_ERR_NONE;
		HWND hWND = NULL;
		mfxHDL handle = NULL;
		POINT pos = { 0 };
		QSVD3D9AllocatorParams* pParams = NULL;
		m_allocator.Reset(new QSVD3D9Allocator());
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
	mfxStatus QSVEncoder::InitializeParam(const TinySize& src, const TinySize& dest)
	{
		m_mfxEncodeParam.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		m_mfxEncodeParam.mfx.FrameInfo.ChromaFormat = FourCCToChroma(MFX_FOURCC_NV12);
		m_mfxEncodeParam.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxEncodeParam.mfx.FrameInfo.Width = MSDK_ALIGN16(static_cast<mfxU16>(dest.cx));
		m_mfxEncodeParam.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxEncodeParam.mfx.FrameInfo.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(dest.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(dest.cy));
		m_mfxEncodeParam.mfx.FrameInfo.CropX = 0;
		m_mfxEncodeParam.mfx.FrameInfo.CropY = 0;
		m_mfxEncodeParam.mfx.FrameInfo.CropW = static_cast<mfxU16>(dest.cx);
		m_mfxEncodeParam.mfx.FrameInfo.CropH = static_cast<mfxU16>(dest.cy);
		m_mfxEncodeParam.AsyncDepth = 4;
		m_mfxEncodeParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		ConvertFrameRate(30.0F, &m_mfxEncodeParam.mfx.FrameInfo.FrameRateExtN, &m_mfxEncodeParam.mfx.FrameInfo.FrameRateExtD);
		m_mfxEncodeParam.mfx.CodecId = MFX_CODEC_AVC;
		m_mfxEncodeParam.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
		m_mfxEncodeParam.mfx.RateControlMethod = MFX_RATECONTROL_CBR;
		m_mfxEncodeParam.mfx.GopRefDist = 0;
		m_mfxEncodeParam.mfx.GopPicSize = 0;
		m_mfxEncodeParam.mfx.NumRefFrame = 0;
		m_mfxEncodeParam.mfx.IdrInterval = 0;
		m_mfxEncodeParam.mfx.CodecProfile = 0;
		m_mfxEncodeParam.mfx.CodecLevel = 0;
		m_mfxEncodeParam.mfx.MaxKbps = CalculateDefaultBitrate(m_mfxEncodeParam.mfx.CodecId, m_mfxEncodeParam.mfx.TargetUsage, m_mfxEncodeParam.mfx.FrameInfo.Width, m_mfxEncodeParam.mfx.FrameInfo.Height, 30);
		m_mfxEncodeParam.mfx.InitialDelayInKB = 0;
		m_mfxEncodeParam.mfx.GopOptFlag = 0;
		m_mfxEncodeParam.mfx.BufferSizeInKB = 0;

		//////////////////////////////////////////////////////////////////////////
		m_mfxVppParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		m_mfxVppParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		ConvertFrameRate(30.0F, &m_mfxVppParam.vpp.In.FrameRateExtN, &m_mfxVppParam.vpp.In.FrameRateExtD);
		m_mfxVppParam.vpp.In.Width = MSDK_ALIGN16(static_cast<mfxU16>(src.cx));
		m_mfxVppParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppParam.vpp.In.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(src.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(src.cy));
		m_mfxVppParam.vpp.In.CropW = static_cast<mfxU16>(src.cx);
		m_mfxVppParam.vpp.In.CropH = static_cast<mfxU16>(src.cy);
		MSDK_MEMCPY_VAR(m_mfxVppParam.vpp.Out, &m_mfxVppParam.vpp.In, sizeof(mfxFrameInfo));
		m_mfxVppParam.vpp.In.FourCC = MFX_FOURCC_RGB4;
		m_mfxVppParam.vpp.Out.FourCC = MFX_FOURCC_NV12;
		m_mfxVppParam.vpp.In.ChromaFormat = FourCCToChroma(MFX_FOURCC_RGB4);
		m_mfxVppParam.vpp.Out.ChromaFormat = FourCCToChroma(MFX_FOURCC_NV12);
		m_mfxVppParam.vpp.Out.Width = MSDK_ALIGN16(static_cast<mfxU16>(dest.cx));
		m_mfxVppParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(dest.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(dest.cy));
		m_vppDoNotUse.NumAlg = 4;
		m_vppDoNotUse.AlgList = new mfxU32[m_vppDoNotUse.NumAlg];
		MSDK_CHECK_POINTER(m_vppDoNotUse.AlgList, MFX_ERR_MEMORY_ALLOC);
		m_vppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		m_vppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		m_vppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		m_vppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;
		m_vppExtParams.push_back((mfxExtBuffer *)&m_vppDoNotUse);
		m_mfxVppParam.ExtParam = &m_vppExtParams[0];
		m_mfxVppParam.NumExtParam = (mfxU16)m_vppExtParams.size();
		m_mfxVppParam.AsyncDepth = m_mfxEncodeParam.AsyncDepth;
		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncoder::AllocFrames()
	{
		MSDK_CHECK_POINTER(m_mfxVideoENCODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
		mfxStatus status = MFX_ERR_NONE;
		mfxFrameAllocRequest request;
		mfxFrameAllocRequest requestVPP[2];
		mfxU16 nEncSurfNum = 0;
		mfxU16 nVppSurfNum = 0;
		MSDK_ZERO_MEMORY(request);
		MSDK_ZERO_MEMORY(requestVPP[0]);
		MSDK_ZERO_MEMORY(requestVPP[1]);
		status = m_mfxVideoENCODE->QueryIOSurf(&m_mfxEncodeParam, &request);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		if (request.NumFrameSuggested < m_mfxEncodeParam.AsyncDepth)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		nEncSurfNum = request.NumFrameSuggested;
		status = m_mfxVideoVPP->QueryIOSurf(&m_mfxVppParam, requestVPP);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		nVppSurfNum = requestVPP[0].NumFrameSuggested;
		nEncSurfNum += nVppSurfNum - m_mfxEncodeParam.AsyncDepth + 1;
		request.NumFrameSuggested = request.NumFrameMin = nEncSurfNum;
		MSDK_MEMCPY_VAR(request.Info, &(m_mfxEncodeParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
		request.Type |= MFX_MEMTYPE_FROM_VPPOUT;
		status = m_allocator->Alloc(m_allocator->pthis, &request, &m_mfxResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		requestVPP[0].NumFrameSuggested = requestVPP[0].NumFrameMin = nVppSurfNum;
		MSDK_MEMCPY_VAR(requestVPP[0].Info, &(m_mfxVppParam.vpp.In), sizeof(mfxFrameInfo));
		status = m_allocator->Alloc(m_allocator->pthis, &(requestVPP[0]), &m_mfxVPPResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		nEncSurfNum = m_mfxResponse.NumFrameActual;
		m_mfxSurfaces.Reset(new mfxFrameSurface1*[nEncSurfNum]);
		if (NULL == m_mfxSurfaces)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		for (mfxU16 i = 0; i < nEncSurfNum; i++)
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
		for (mfxU16 i = 0; i < nVppSurfNum; i++)
		{
			m_mfxVPPSurfaces[i] = new mfxFrameSurface1();
			MSDK_MEMCPY_VAR(m_mfxVPPSurfaces[i]->Info, &requestVPP[1].Info, sizeof(mfxFrameInfo));
			m_mfxVPPSurfaces[i]->Data.MemId = m_mfxVPPResponse.mids[i];
		}
	_ERROR:
		return status;
	}
	void QSVEncoder::DeleteFrames()
	{
		for (mfxU16 i = 0; i < m_mfxVPPResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxVPPSurfaces[i]);
		}
		m_mfxVPPSurfaces.Reset(NULL);
		for (mfxU16 i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			SAFE_DELETE(m_mfxSurfaces[i]);
		}
		m_mfxSurfaces.Reset(NULL);
		if (m_allocator != NULL)
		{
			m_allocator->Free(m_allocator->pthis, &m_mfxResponse);
			m_allocator->Free(m_allocator->pthis, &m_mfxVPPResponse);
		}
		ZeroMemory(&m_mfxResponse, sizeof(m_mfxResponse));
		ZeroMemory(&m_mfxVPPResponse, sizeof(m_mfxVPPResponse));
	}
	void QSVEncoder::DeleteAllocator()
	{
		m_allocator.Reset(NULL);
		m_qsvd3d.Reset(NULL);
	}
}
