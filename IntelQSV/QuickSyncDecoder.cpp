#include "stdafx.h"
#include "QuickSyncDecoder.h"

namespace QSV
{
	QuickSyncDecoder::QuickSyncDecoder(const QSVConfig& config)
		:m_pVideoParams(NULL),
		m_config(config),
		m_bAcceleration(FALSE),
		m_bUseD3DAlloc(FALSE),
		m_bUseD3D11Alloc(FALSE),
		m_assists(0)
	{

	}
	QuickSyncDecoder::~QuickSyncDecoder()
	{

	}
	mfxStatus QuickSyncDecoder::Initialize(mfxVideoParam* pVideoParams, mfxU32 sPitch)
	{
		mfxStatus status = MFX_ERR_NONE;
		mfxIMPL sIMPL = MFX_IMPL_AUTO_ANY;
		m_mfxVersion.Major = MIN_REQUIRED_API_VER_MAJOR;
		m_mfxVersion.Minor = MIN_REQUIRED_API_VER_MINOR;
		INT adapter = 0;
		if (sIMPL != MFX_IMPL_SOFTWARE)
		{
			adapter = GetIntelAdapter(NULL);
			if (adapter >= 0)
			{
				sIMPL |= MFX_IMPL_VIA_D3D9;
			}
			else if (m_config.EnableD3D11)
			{
				sIMPL |= MFX_IMPL_VIA_D3D11;
			}
			else
			{
				LOG(ERROR) << "QsDecoder: Can't create HW decoder, the iGPU is not connected to a screen!\n";
				status = MFX_ERR_UNSUPPORTED;
			}
		}
		status = InitializeSession(sIMPL);
		if (MSDK_SUCCEEDED(status) && !m_config.EnableD3D11 && 0 > adapter)
		{
			LOG(ERROR) << "QsDecoder: can't create HW decoder, the iGPU is not connected to a screen!\n";
			status = MFX_ERR_UNSUPPORTED;
		}
		return status;
	}
	mfxStatus QuickSyncDecoder::InitializeSession(mfxIMPL impl)
	{
		if (m_mfxVideoSession != NULL)
			return MFX_ERR_NONE;
		m_mfxVideoSession.Reset(new MFXVideoSession());
		if (m_mfxVideoSession != NULL)
			return MFX_ERR_MEMORY_ALLOC;
		mfxStatus status = m_mfxVideoSession->Init(impl, &m_mfxVersion);
		if (MSDK_FAILED(status))
		{
			LOG(ERROR) << "QsDecoder: failed to initialize MSDK session!\n";
			return status;
		}
		m_mfxVideoSession->QueryIMPL(&m_mfxIMPL);
		m_mfxVideoSession->QueryVersion(&m_mfxVersion);
		m_bAcceleration = m_mfxIMPL != MFX_IMPL_SOFTWARE;
		m_bUseD3DAlloc = m_bAcceleration;
		m_bUseD3D11Alloc = m_bUseD3DAlloc && ((m_mfxIMPL & MFX_IMPL_VIA_D3D11) == MFX_IMPL_VIA_D3D11);
		m_mfxVideoDECODE.Reset(new  MFXVideoDECODE((mfxSession)*m_mfxVideoSession));
		if (m_mfxVideoDECODE != NULL)
			return MFX_ERR_MEMORY_ALLOC;
#if MFX_D3D11_SUPPORT
		if (m_bUseD3D11Alloc)
		{
			INT adapterID = GetMSDKAdapters(*m_mfxVideoSession);
			if (NULL == m_device)
			{
				m_qsvd3d.Reset(new QSVD3D11());
				if (m_qsvd3d != NULL)
					return MFX_ERR_MEMORY_ALLOC;
				if (MSDK_FAILED(status = m_qsvd3d->Initialize(adapterID)))
				{
					LOG(ERROR) << "QsDecoder: D3D11 init have failed!\n";
					m_qsvd3d.Reset(NULL);
					return status;
				}
			}
			mfxHDL hdl = m_qsvd3d->GetHandle(MFX_HANDLE_D3D11_DEVICE);
			status = m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D11_DEVICE, hdl);
		}
#endif
		MSDK_ZERO_MEMORY((void*)&m_locks, sizeof(m_locks));
		return MFX_ERR_NONE;
	}
	mfxStatus QuickSyncDecoder::Decode(mfxBitstream* pI, mfxFrameSurface1*& pO)
	{
		return MFX_ERR_NONE;
	}
	void QuickSyncDecoder::SetAssists(mfxU16 count)
	{
		if (m_assists != count)
		{
			m_assists = count;
			FreeFrames();
		}
	}
	mfxStatus QuickSyncDecoder::AllocFrames(mfxVideoParam* pVideoParams, mfxU32 sPitch)
	{
		LOG(INFO) << "QsDecoder: AllocFrames\n";
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NOT_INITIALIZED);
		if (m_surfaces)
			return MFX_ERR_NONE;
		mfxStatus status = MFX_ERR_NONE;
		status = CreateAllocator();
		MSDK_CHECK_NOT_EQUAL(status, MFX_ERR_NONE, status);
		mfxFrameAllocRequest allocRequest;
		memset(&allocRequest, 0, sizeof(allocRequest));
		status = m_mfxVideoDECODE->QueryIOSurf(pVideoParams, &allocRequest);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		MSDK_CHECK_RESULT_P_RET(status, MFX_ERR_NONE);
		allocRequest.NumFrameSuggested = (mfxU16)m_assists + allocRequest.NumFrameSuggested;
		allocRequest.NumFrameMin = allocRequest.NumFrameSuggested;
		allocRequest.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE;
		allocRequest.Type |= (m_bUseD3DAlloc) ? MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET : MFX_MEMTYPE_SYSTEM_MEMORY;
		memcpy(&allocRequest.Info, &pVideoParams->mfx.FrameInfo, sizeof(mfxFrameInfo));
		allocRequest.Info.Height = MSDK_ALIGN32(allocRequest.Info.Height);
		allocRequest.Info.Width = (mfxU16)sPitch;
		status = m_allocator->Alloc(m_allocator->pthis, &allocRequest, &m_allocResponse);
		MSDK_CHECK_RESULT_P_RET(status, MFX_ERR_NONE);
		m_requests = m_allocResponse.NumFrameActual;
		ASSERT(m_requests == allocRequest.NumFrameSuggested);
		m_surfaces.Reset(new mfxFrameSurface1[m_requests]);
		MSDK_CHECK_POINTER(m_surfaces, MFX_ERR_MEMORY_ALLOC);
		MSDK_ZERO_MEMORY(m_surfaces, sizeof(mfxFrameSurface1) * m_requests);
		for (mfxU32 i = 0; i < m_requests; ++i)
		{
			memcpy(&(m_surfaces[i].Info), &pVideoParams->mfx.FrameInfo, sizeof(mfxFrameInfo));
			m_surfaces[i].Data.MemId = m_allocResponse.mids[i];
			m_surfaces[i].Data.Pitch = (mfxU16)sPitch;
		}
		return status;
	}
	mfxStatus QuickSyncDecoder::FreeFrames()
	{
		mfxStatus status = MFX_ERR_NONE;
		if (m_allocator != NULL && m_allocResponse.NumFrameActual > 0)
		{
			status = m_allocator->Free(m_allocator->pthis, &m_allocResponse);
			memset(&m_allocResponse, 0, sizeof(m_allocResponse));
		}
		m_requests = 0;
		m_surfaces.Reset(NULL);
		return MFX_ERR_NONE;
	}
	mfxStatus QuickSyncDecoder::Reset(mfxVideoParam* pVideoParams, mfxU32 sPitch)
	{
		MSDK_CHECK_POINTER(pVideoParams, MFX_ERR_NULL_PTR);
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NOT_INITIALIZED);
		mfxStatus status = MFX_ERR_NONE;
		if (m_allocator != NULL)
		{
			status = m_mfxVideoDECODE->Reset(pVideoParams);
			if (MSDK_FAILED(status))
			{
				m_mfxVideoDECODE->Close();
				FreeFrames();
			}
			if (m_surfaces != NULL)
			{
				for (INT i = 0; i < m_requests; ++i)
				{
					m_surfaces[i].Data.Locked = 0;
					m_locks[i] = 0;
				}
			}
		}
		status = AllocFrames(pVideoParams, sPitch);
		MSDK_CHECK_RESULT_P_RET(status, MFX_ERR_NONE);
		status = m_mfxVideoDECODE->Init(pVideoParams);
		switch (status)
		{
		case MFX_ERR_NONE:
			LOG(INFO) << "QsDecoder: decoder Init is successful\n";
			break;
		case MFX_WRN_PARTIAL_ACCELERATION:
			LOG(INFO) << "QsDecoder: decoder Init is successful w/o HW acceleration\n";
			m_bAcceleration = FALSE;
			break;
		case MFX_WRN_INCOMPATIBLE_VIDEO_PARAM:
			LOG(INFO) << "QsDecoder: decoder Init is successful - wrong video parameters\n";
			break;
		default:
			LOG(ERROR) << "QsDecoder: decoder Init has failed!\n";
			break;
		}
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		m_pVideoParams = pVideoParams;
		return status;
	}
	mfxStatus QuickSyncDecoder::CreateAllocator()
	{
		if (m_allocator != NULL)
			return MFX_ERR_NONE;
		if (!m_pVideoParams)
			return MFX_ERR_NOT_INITIALIZED;
		LOG(INFO) << "QuickSyncDecoder: CreateAllocator\n";
		TinyScopedPtr<mfxAllocatorParams> allocatorParams;
		mfxStatus status = MFX_ERR_NONE;
		if (m_bUseD3DAlloc)
		{
			m_pVideoParams->IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY | MFX_IOPATTERN_IN_VIDEO_MEMORY;
			INT adapterID = GetMSDKAdapters(*m_mfxVideoSession);
			if (m_bUseD3D11Alloc)//Ê¹ÓÃDX11
			{
#if MFX_D3D11_SUPPORT
				ASSERT(m_qsvd3d);
				MSDK_CHECK_POINTER(m_qsvd3d, MFX_ERR_NULL_PTR);
				QSVD3D11AllocatorParams* ps = new QSVD3D11AllocatorParams;
				if (ps != NULL)
					return MFX_ERR_MEMORY_ALLOC;
				ps->pDevice = (ID3D11Device*)m_qsvd3d->GetHandle(MFX_HANDLE_D3D11_DEVICE);
				allocatorParams.Reset(ps);
				m_allocator.Reset(new QSVD3D11Allocator());
				if (m_allocator != NULL)
					return MFX_ERR_MEMORY_ALLOC;
#endif
			}
			else
			{
				m_qsvd3d.Reset(new QSVD3D9());
				if (m_qsvd3d != NULL)
					return MFX_ERR_MEMORY_ALLOC;
				POINT pos = { 0 };
				HWND hWND = ::WindowFromPoint(pos);
				if (!hWND)
				{
					hWND = GetDesktopWindow();
				}
				if (MSDK_FAILED(status = m_qsvd3d->Initialize(hWND, adapterID)))
				{
					LOG(ERROR) << "QsDecoder: D3D9 init have failed!\n";
					m_qsvd3d.Reset(NULL);
					return status;
				}
				mfxHDL hdl = m_qsvd3d->GetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER);
				status = m_mfxVideoSession->SetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, hdl);
				MSDK_CHECK_NOT_EQUAL(status, MFX_ERR_NONE, status);
				QSVD3D9AllocatorParams* ps = new QSVD3D9AllocatorParams;
				ps->pManager = (IDirect3DDeviceManager9*)hdl;
				allocatorParams.Reset(ps);
				m_allocator.Reset(new QSVD3D9Allocator());
				if (m_allocator != NULL)
					return MFX_ERR_MEMORY_ALLOC;
			}
		}
		else
		{
			m_bUseD3DAlloc = FALSE;
			m_pVideoParams->IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY | MFX_IOPATTERN_IN_SYSTEM_MEMORY;
			m_allocator.Reset(new QSVMemeryAllocator());
			if (m_allocator != NULL)
				return MFX_ERR_MEMORY_ALLOC;
		}
		status = m_allocator->Initialize(allocatorParams.Ptr());
		if (MSDK_SUCCEEDED(status))
		{
			status = m_mfxVideoSession->SetFrameAllocator(m_allocator);
			if (MSDK_FAILED(status))
			{
				LOG(ERROR) << "QsDecoder: Session SetFrameAllocator failed!\n";
			}
		}
		else
		{
			LOG(ERROR) << "QsDecoder: Allocator Init failed!\n";
			m_allocator.Reset(NULL);
		}
		return status;
	}
	mfxStatus QuickSyncDecoder::DestoryAllocator()
	{
		if (m_allocator != NULL)
			m_allocator->Close();
		m_allocator.Reset(NULL);
		if (m_qsvd3d != NULL)
			m_qsvd3d->Close();
		m_qsvd3d.Reset(NULL);
		return MFX_ERR_NONE;
	}
}

