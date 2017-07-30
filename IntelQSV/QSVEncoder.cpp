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
		:m_dwINC(0)
	{
		ZeroMemory(&m_mfxResidial, sizeof(m_mfxResidial));
		ZeroMemory(&m_mfxEncodeCtrl, sizeof(m_mfxEncodeCtrl));
	}
	QSVEncoder::~QSVEncoder()
	{
	}
	BOOL QSVEncoder::Open(const TinySize& src, const TinySize& dest, DWORD dwBitRate, DWORD dwFrameRate)
	{
		mfxStatus status = MFX_ERR_NONE;
		m_streamBits[0].Reset(new BYTE[1920 * 1080 * 4]);
		m_streamBits[1].Reset(new BYTE[1920 * 1080 * 4]);
		m_mfxResidial.MaxLength = 1920 * 1080 * 4;
		m_mfxResidial.Data = m_streamBits[0];
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
		status = InitializeParam(src, dest, dwBitRate, dwFrameRate);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = CreateAllocator(src, dest);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = AllocFrames();
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoENCODE->Init(&m_mfxVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoVPP->Init(&m_mfxVppVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoVPP->GetVideoParam(&m_mfxVppVideoParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		MSDK_ZERO_MEMORY(m_mfxCodingOption);
		m_mfxCodingOption.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
		m_mfxCodingOption.Header.BufferSz = sizeof(m_mfxCodingOption);
		m_mfxCodingOption.ViewOutput = MFX_CODINGOPTION_ON;
		m_encExtParams.push_back((mfxExtBuffer *)&m_mfxCodingOption);
		m_mfxVideoParam.ExtParam = &m_encExtParams[0];
		m_mfxVideoParam.NumExtParam = (mfxU16)m_encExtParams.size();
		MSDK_ZERO_MEMORY(m_mfxCodingOptionSPSPPS);
		mfxU8	mfxsps[128];
		mfxU8	mfxpps[128];
		m_mfxCodingOptionSPSPPS.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
		m_mfxCodingOptionSPSPPS.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);
		m_mfxCodingOptionSPSPPS.PPSBuffer = mfxpps;
		m_mfxCodingOptionSPSPPS.PPSBufSize = 128;
		m_mfxCodingOptionSPSPPS.SPSBuffer = mfxsps;
		m_mfxCodingOptionSPSPPS.SPSBufSize = 128;
		m_encExtParams.push_back((mfxExtBuffer *)&m_mfxCodingOption);
		m_encExtParams.push_back((mfxExtBuffer *)&m_mfxCodingOptionSPSPPS);
		m_mfxVideoParam.ExtParam = &m_encExtParams[0];
		m_mfxVideoParam.NumExtParam = (mfxU16)m_encExtParams.size();
		status = m_mfxVideoENCODE->GetVideoParam(&m_mfxVideoParam);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		m_mfxSPS.resize(m_mfxCodingOptionSPSPPS.SPSBufSize);
		memcpy(&m_mfxSPS[0], m_mfxCodingOptionSPSPPS.SPSBuffer, m_mfxCodingOptionSPSPPS.SPSBufSize);
		m_mfxPPS.resize(m_mfxCodingOptionSPSPPS.PPSBufSize);
		memcpy(&m_mfxPPS[0], m_mfxCodingOptionSPSPPS.PPSBuffer, m_mfxCodingOptionSPSPPS.PPSBufSize);
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
		MSDK_SAFE_DELETE_ARRAY(m_vppDoNotUse.AlgList);
		DeleteFrames();
		DeleteAllocator();
	}
	vector<mfxU8>& QSVEncoder::GetSPS()
	{
		return m_mfxSPS;
	}
	vector<mfxU8>& QSVEncoder::GetPPS()
	{
		return m_mfxPPS;
	}
	BOOL QSVEncoder::Encode(SampleTag& tag, BYTE*& bo, LONG& so, MediaTag& mediaTag)
	{
		MSDK_CHECK_POINTER(m_mfxVideoENCODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
		return Process(tag, bo, so, mediaTag) == MFX_ERR_NONE;
	}
	void QSVEncoder::LoadRGB32(mfxFrameSurface1* pIN, const BYTE* bits, LONG size, LONG timestamp)
	{
		m_allocator->Lock(m_allocator->pthis, pIN->Data.MemId, &(pIN->Data));
		mfxU8 *pRGB = pIN->Data.B;
		pRGB = pRGB + pIN->Info.CropX + pIN->Info.CropY * pIN->Data.Pitch;
		for (mfxU16 i = 0; i < pIN->Info.Height; i++)
		{
			memcpy(pRGB + i * pIN->Data.Pitch, bits + i * 4 * pIN->Info.Width, 4 * pIN->Info.Width);
		}
		pIN->Data.TimeStamp = timestamp * 90000 / m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN;
		m_allocator->Unlock(m_allocator->pthis, pIN->Data.MemId, &(pIN->Data));
	}
	mfxStatus QSVEncoder::Process(SampleTag& tag, BYTE*& bo, LONG& so, MediaTag& mediaTag)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxSyncPoint syncpVPP = NULL;
		mfxSyncPoint syncpVideo = NULL;
		INT index = GetFreeVPPSurfaceIndex();
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
		INT index1 = GetFreeVideoSurfaceIndex();
		MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index1, MFX_ERR_MEMORY_ALLOC);
		do
		{
			mfxFrameSurface1* pVPPIN = m_mfxVPPSurfaces[index];
			this->LoadRGB32(pVPPIN, tag.bits, tag.size, tag.samplePTS);
			status = m_mfxVideoVPP->RunFrameVPPAsync(m_mfxVPPSurfaces[index], m_mfxSurfaces[index1], NULL, &syncpVPP);
			if (MFX_ERR_MORE_SURFACE == status)
			{
				index = GetFreeVPPSurfaceIndex();
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, index, MFX_ERR_MEMORY_ALLOC);
				index1 = GetFreeVideoSurfaceIndex();
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
				status = m_mfxVideoENCODE->EncodeFrameAsync(NULL, m_mfxSurfaces[index1], &m_mfxResidial, &syncpVideo);
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
			if (MFX_ERR_NONE < status && syncpVideo)
			{
				status = MFX_ERR_NONE;
			}
			if (MFX_ERR_NONE == status)
			{
				status = m_mfxSession.SyncOperation(syncpVideo, 1000);
				if (status == MFX_ERR_NONE)
				{
					m_dwINC++;
					memcpy(m_streamBits[1], m_mfxResidial.Data + m_mfxResidial.DataOffset, m_mfxResidial.DataLength);
					bo = m_streamBits[1];
					so = m_mfxResidial.DataLength;
					mediaTag.DTS = m_mfxResidial.DecodeTimeStamp * m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN / 90000;
					mediaTag.PTS = m_mfxResidial.TimeStamp * m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN / 90000;
					mediaTag.INC = m_dwINC;
					mediaTag.dwType = 0;
					mediaTag.dwFlag = m_mfxResidial.FrameType & (MFX_FRAMETYPE_I | MFX_FRAMETYPE_IDR) ? 0x17 : 0x27;
					m_mfxResidial.DataLength = 0;
				}
				return status;
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
		MSDK_CHECK_POINTER(m_mfxVideoENCODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
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
	mfxStatus QSVEncoder::InitializeParam(const TinySize& src, const TinySize& dest, DWORD dwBitRate, DWORD dwFrameRate)
	{
		MSDK_CHECK_POINTER(m_mfxVideoENCODE, MFX_ERR_MEMORY_ALLOC);
		MSDK_CHECK_POINTER(m_mfxVideoVPP, MFX_ERR_MEMORY_ALLOC);
		mfxStatus status = MFX_ERR_NONE;
		ZeroMemory(&m_mfxVideoParam, sizeof(m_mfxVideoParam));
		m_mfxVideoParam.AsyncDepth = 4;
		m_mfxVideoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		m_mfxVideoParam.mfx.GopRefDist = 1 + 3;// I帧或P帧之间的距离
		m_mfxVideoParam.mfx.GopPicSize = 1 + 3;//每秒I帧个数
		m_mfxVideoParam.mfx.NumRefFrame = 0;
		m_mfxVideoParam.mfx.IdrInterval = 0;
		m_mfxVideoParam.mfx.CodecProfile = 0;
		m_mfxVideoParam.mfx.CodecLevel = 0;
		m_mfxVideoParam.mfx.MaxKbps = 0;
		m_mfxVideoParam.mfx.InitialDelayInKB = 0;
		m_mfxVideoParam.mfx.GopOptFlag = 0;
		m_mfxVideoParam.mfx.BufferSizeInKB = 0;
		m_mfxVideoParam.mfx.EncodedOrder = 0;
		m_mfxVideoParam.mfx.NumSlice = 1;
		m_mfxVideoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_mfxVideoParam.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
		m_mfxVideoParam.mfx.TargetKbps = static_cast<mfxU16>(dwBitRate);
		m_mfxVideoParam.mfx.RateControlMethod = MFX_RATECONTROL_CBR;
		m_mfxVideoParam.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		m_mfxVideoParam.mfx.FrameInfo.ChromaFormat = FourCCToChroma(MFX_FOURCC_NV12);
		m_mfxVideoParam.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxVideoParam.mfx.FrameInfo.Width = MSDK_ALIGN16(static_cast<mfxU16>(dest.cx));
		m_mfxVideoParam.mfx.FrameInfo.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVideoParam.mfx.FrameInfo.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(dest.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(dest.cy));
		m_mfxVideoParam.mfx.FrameInfo.CropX = 0;
		m_mfxVideoParam.mfx.FrameInfo.CropY = 0;
		m_mfxVideoParam.mfx.FrameInfo.CropW = static_cast<mfxU16>(dest.cx);
		m_mfxVideoParam.mfx.FrameInfo.CropH = static_cast<mfxU16>(dest.cy);
		m_mfxVideoParam.mfx.FrameInfo.FrameRateExtN = static_cast<mfxU16>(dwFrameRate);
		m_mfxVideoParam.mfx.FrameInfo.FrameRateExtD = 1;
		status = m_mfxVideoENCODE->Query(&m_mfxVideoParam, &m_mfxVideoParam);
		if (status != MFX_ERR_NONE)
			return status;
		//////////////////////////////////////////////////////////////////////////
		ZeroMemory(&m_mfxVppVideoParam, sizeof(m_mfxVppVideoParam));
		m_mfxVppVideoParam.AsyncDepth = m_mfxVideoParam.AsyncDepth;
		m_mfxVppVideoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		m_mfxVppVideoParam.vpp.In.FourCC = MFX_FOURCC_RGB4;
		m_mfxVppVideoParam.vpp.In.ChromaFormat = FourCCToChroma(MFX_FOURCC_RGB4);
		m_mfxVppVideoParam.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxVppVideoParam.vpp.In.Width = MSDK_ALIGN16(static_cast<mfxU16>(src.cx));
		m_mfxVppVideoParam.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppVideoParam.vpp.In.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(src.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(src.cy));
		m_mfxVppVideoParam.vpp.In.CropW = static_cast<mfxU16>(src.cx);
		m_mfxVppVideoParam.vpp.In.CropH = static_cast<mfxU16>(src.cy);
		m_mfxVppVideoParam.vpp.In.CropX = 0;
		m_mfxVppVideoParam.vpp.In.CropY = 0;
		m_mfxVppVideoParam.vpp.In.FrameRateExtN = static_cast<mfxU16>(dwFrameRate);
		m_mfxVppVideoParam.vpp.In.FrameRateExtD = 1;
		MSDK_MEMCPY_VAR(m_mfxVppVideoParam.vpp.Out, &m_mfxVppVideoParam.vpp.In, sizeof(mfxFrameInfo));
		m_mfxVppVideoParam.vpp.Out.FourCC = MFX_FOURCC_NV12;
		m_mfxVppVideoParam.vpp.Out.ChromaFormat = FourCCToChroma(MFX_FOURCC_NV12);
		m_mfxVppVideoParam.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_mfxVppVideoParam.vpp.Out.Width = MSDK_ALIGN16(static_cast<mfxU16>(dest.cx));
		m_mfxVppVideoParam.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppVideoParam.vpp.Out.PicStruct) ? MSDK_ALIGN16(static_cast<mfxU16>(dest.cy)) : MSDK_ALIGN32(static_cast<mfxU16>(dest.cy));
		m_mfxVppVideoParam.vpp.Out.CropW = static_cast<mfxU16>(dest.cx);
		m_mfxVppVideoParam.vpp.Out.CropH = static_cast<mfxU16>(dest.cy);
		m_mfxVppVideoParam.vpp.Out.CropX = 0;
		m_mfxVppVideoParam.vpp.Out.CropY = 0;
		m_mfxVppVideoParam.vpp.Out.FrameRateExtN = static_cast<mfxU16>(dwFrameRate);
		m_mfxVppVideoParam.vpp.Out.FrameRateExtD = 1;
		ZeroMemory(&m_vppDoNotUse, sizeof(m_vppDoNotUse));
		m_vppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
		m_vppDoNotUse.Header.BufferSz = sizeof(m_vppDoNotUse);
		m_vppDoNotUse.NumAlg = 4;
		m_vppDoNotUse.AlgList = new mfxU32[m_vppDoNotUse.NumAlg];
		MSDK_CHECK_POINTER(m_vppDoNotUse.AlgList, MFX_ERR_MEMORY_ALLOC);
		m_vppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;
		m_vppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;
		m_vppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;
		m_vppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;
		m_vppExtParams.push_back((mfxExtBuffer *)&m_vppDoNotUse);
		m_mfxVppVideoParam.ExtParam = &m_vppExtParams[0];
		m_mfxVppVideoParam.NumExtParam = (mfxU16)m_vppExtParams.size();
		status = m_mfxVideoVPP->Query(&m_mfxVppVideoParam, &m_mfxVppVideoParam);
		if (status != MFX_ERR_NONE)
			return status;
		return status;
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
		status = m_mfxVideoENCODE->QueryIOSurf(&m_mfxVideoParam, &request);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		if (request.NumFrameSuggested < m_mfxVideoParam.AsyncDepth)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		nEncSurfNum = request.NumFrameSuggested;
		status = m_mfxVideoVPP->QueryIOSurf(&m_mfxVppVideoParam, requestVPP);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		nVppSurfNum = requestVPP[0].NumFrameSuggested;
		nEncSurfNum += nVppSurfNum - m_mfxVideoParam.AsyncDepth + 1;
		request.NumFrameSuggested = request.NumFrameMin = nEncSurfNum;
		MSDK_MEMCPY_VAR(request.Info, &(m_mfxVideoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
		request.Type |= MFX_MEMTYPE_FROM_VPPOUT;
		status = m_allocator->Alloc(m_allocator->pthis, &request, &m_mfxResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		MSDK_MEMCPY_VAR(requestVPP[0].Info, &(m_mfxVppVideoParam.vpp.In), sizeof(mfxFrameInfo));
		status = m_allocator->Alloc(m_allocator->pthis, &(requestVPP[0]), &m_mfxVPPResponse);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		m_mfxSurfaces.Reset(new mfxFrameSurface1*[m_mfxResponse.NumFrameActual]);
		if (NULL == m_mfxSurfaces)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		for (mfxU16 i = 0; i < m_mfxResponse.NumFrameActual; i++)
		{
			m_mfxSurfaces[i] = new mfxFrameSurface1();
			memset(m_mfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
			MSDK_MEMCPY_VAR(m_mfxSurfaces[i]->Info, &(m_mfxVideoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_mfxSurfaces[i]->Data.MemId = m_mfxResponse.mids[i];
		}
		m_mfxVPPSurfaces.Reset(new mfxFrameSurface1*[m_mfxVPPResponse.NumFrameActual]);
		if (NULL == m_mfxVPPSurfaces)
		{
			status = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		for (mfxU16 i = 0; i < m_mfxVPPResponse.NumFrameActual; i++)
		{
			m_mfxVPPSurfaces[i] = new mfxFrameSurface1();
			memset(m_mfxVPPSurfaces[i], 0, sizeof(mfxFrameSurface1));
			MSDK_MEMCPY_VAR(m_mfxVPPSurfaces[i]->Info, &m_mfxVppVideoParam.mfx.FrameInfo, sizeof(mfxFrameInfo));
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
