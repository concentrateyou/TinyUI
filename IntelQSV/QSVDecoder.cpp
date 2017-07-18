#include "stdafx.h"
#include "QSVDecoder.h"

namespace QSV
{
	QSVDecoder::QSVDecoder()
	{
	}

	QSVDecoder::~QSVDecoder()
	{
	}
	BOOL QSVDecoder::Open(const BYTE* bits, LONG size)
	{
		mfxStatus sts = MFX_ERR_NONE;
		mfxInitParam initParam = { 0 };
		initParam.Implementation |= MFX_IMPL_HARDWARE_ANY | MFX_IMPL_VIA_D3D9;;
		sts = m_mfxSession.InitEx(initParam);
		if (MFX_ERR_NONE != sts)
			goto _ERROR;
		sts = m_mfxSession.QueryVersion(&m_mfxVersion);
		if (MFX_ERR_NONE != sts)
			goto _ERROR;
		sts = m_mfxSession.QueryIMPL(&m_mfxImpl);
		if (MFX_ERR_NONE != sts)
			goto _ERROR;
		m_mfxVideoDECODE.Reset(new MFXVideoDECODE(m_mfxSession));
		if (NULL == m_mfxVideoDECODE)
		{
			sts = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_mfxVideoVPP.Reset(new MFXVideoVPP(m_mfxSession));
		if (NULL == m_mfxVideoVPP)
		{
			sts = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		ZeroMemory(&m_mfxBitstream, sizeof(m_mfxBitstream));
		m_mfxBitstream.Data = new mfxU8[MAX_ALLOC_SIZE];
		if (NULL == m_mfxBitstream.Data)
		{
			sts = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
		m_mfxBitstream.MaxLength = MAX_ALLOC_SIZE;
		sts = InitializeMFXParam(bits, size);
		if (MFX_ERR_NONE != sts)
			goto _ERROR;
		m_mfxVideoVPP.Reset(new MFXVideoVPP(m_mfxSession));
		if (NULL == m_mfxVideoVPP)
		{
			sts = MFX_ERR_MEMORY_ALLOC;
			goto _ERROR;
		}
	_ERROR:
		return sts == MFX_ERR_NONE;
	}
	mfxStatus QSVDecoder::InitializeMFXParam(const BYTE* bits, LONG size)
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		memset(&m_mfxVideoParam, 0, sizeof(m_mfxVideoParam));
		m_mfxVideoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_mfxVideoParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		memcpy_s(m_mfxBitstream.Data, size, bits, size);
		m_mfxBitstream.DataLength += size;
		status = m_mfxVideoDECODE->DecodeHeader(&m_mfxBitstream, &m_mfxVideoParam);
		if (MFX_ERR_MORE_DATA == status)
		{
			mfxU16 oldFlag = m_mfxBitstream.DataFlag;
			m_mfxBitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
			status = m_mfxVideoDECODE->DecodeHeader(&m_mfxBitstream, &m_mfxVideoParam);
			m_mfxBitstream.DataFlag = oldFlag;
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

	mfxStatus QSVDecoder::AllocFrames()
	{
		MSDK_CHECK_POINTER(m_mfxVideoDECODE, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		mfxFrameAllocRequest Request;
		mfxFrameAllocRequest VppRequest[2];
		mfxU16 nSurfNum = 0;
		mfxU16 nVppSurfNum = 0;
		MSDK_ZERO_MEMORY(Request);
		MSDK_ZERO_MEMORY(VppRequest[0]);
		MSDK_ZERO_MEMORY(VppRequest[1]);
		status = m_mfxVideoDECODE->Query(&m_mfxVideoParam, &m_mfxVideoParam);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
		status = m_mfxVideoDECODE->QueryIOSurf(&m_mfxVideoParam, &Request);
		MSDK_IGNORE_MFX_STS(status, MFX_WRN_PARTIAL_ACCELERATION);
		if (MFX_ERR_NONE != status)
			goto _ERROR;
	_ERROR:
		return status;
	}
}
