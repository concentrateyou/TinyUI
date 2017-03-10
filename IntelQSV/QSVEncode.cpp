#include "stdafx.h"
#include "QSVEncode.h"

namespace QSV
{
	QSVEncode::QSVEncode()
	{
	}

	QSVEncode::~QSVEncode()
	{
	}
	BOOL QSVEncode::Initialize(const QSVParam& param)
	{
		if (!IntelExists())
			return FALSE;
		mfxStatus sts = MFX_ERR_NONE;
		mfxIMPL impl = MFX_IMPL_AUTO_ANY | MFX_IMPL_VIA_D3D11;
		mfxVersion ver = { { 1, 1 } };
		sts = ::Initialize(impl, ver, &m_session, &m_allocator);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		m_videoENCODE.Reset(new MFXVideoENCODE(m_session));
		ZeroMemory(&m_videoParam, sizeof(m_videoParam));
		m_videoParam.mfx.CodecId = MFX_CODEC_AVC;
		m_videoParam.mfx.GopOptFlag = MFX_GOP_STRICT;
		m_videoParam.mfx.NumSlice = 1;
		m_videoParam.mfx.CodecProfile = MFX_PROFILE_AVC_BASELINE;
		m_videoParam.mfx.TargetUsage = MFX_TARGETUSAGE_BEST_SPEED;
		m_videoParam.mfx.FrameInfo.FrameRateExtN = param.dwFPS;
		m_videoParam.mfx.FrameInfo.FrameRateExtD = 1;
		m_videoParam.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		m_videoParam.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		m_videoParam.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		m_videoParam.mfx.FrameInfo.CropX = 0;
		m_videoParam.mfx.FrameInfo.CropY = 0;
		m_videoParam.mfx.FrameInfo.CropW = param.dwCX;
		m_videoParam.mfx.FrameInfo.CropH = param.dwCY;
		m_videoParam.mfx.RateControlMethod = param.dwRC;
		switch (param.dwRC)
		{
		case MFX_RATECONTROL_LA:
		case MFX_RATECONTROL_CBR://恒定码率
			m_videoParam.mfx.TargetKbps = param.dwKbps;
			break;
		case MFX_RATECONTROL_VBR:
		case MFX_RATECONTROL_VCM://
			m_videoParam.mfx.TargetKbps = param.dwKbps;
			m_videoParam.mfx.MaxKbps = param.dwMaxKbps;
			break;
		case MFX_RATECONTROL_CQP://恒定质量
			m_videoParam.mfx.QPI = param.dwQPI;
			m_videoParam.mfx.QPB = param.dwQPB;
			m_videoParam.mfx.QPP = param.dwQPP;
			break;
		case MFX_RATECONTROL_AVBR://平均码率
			m_videoParam.mfx.TargetKbps = param.dwKbps;
			m_videoParam.mfx.Accuracy = param.dwAccuracy;
			m_videoParam.mfx.Convergence = param.dwConvergence;
			break;
		case MFX_RATECONTROL_ICQ:
		case MFX_RATECONTROL_LA_ICQ:
			m_videoParam.mfx.ICQQuality = param.dwICQQuality;
			break;
		default:
			break;
		}
		m_videoParam.AsyncDepth = param.dwAsyncDepth;
		m_videoParam.mfx.GopPicSize = param.dwGopPicSize;
		static mfxExtBuffer* extendedBuffers[2];
		INT iBuffers = 0;
		if (param.dwAsyncDepth == 1)
		{
			m_videoParam.mfx.NumRefFrame = 1;
			m_videoParam.mfx.GopRefDist = 1;
			memset(&m_co, 0, sizeof(mfxExtCodingOption));
			m_co.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
			m_co.Header.BufferSz = sizeof(mfxExtCodingOption);
			m_co.MaxDecFrameBuffering = 1;
			extendedBuffers[iBuffers++] = (mfxExtBuffer*)&m_co;
		}
		else
		{
			m_videoParam.mfx.GopRefDist = param.dwGopRefDist;
		}
		if (param.dwRC == MFX_RATECONTROL_LA_ICQ || param.dwRC == MFX_RATECONTROL_LA)
		{
			memset(&m_co2, 0, sizeof(mfxExtCodingOption2));
			m_co2.Header.BufferId = MFX_EXTBUFF_CODING_OPTION;
			m_co2.Header.BufferSz = sizeof(m_co2);
			m_co2.LookAheadDepth = param.dwLookAheadDepth;
			extendedBuffers[iBuffers++] = (mfxExtBuffer*)& m_co2;
		}
		if (iBuffers > 0)
		{
			m_videoParam.ExtParam = extendedBuffers;
			m_videoParam.NumExtParam = (mfxU16)iBuffers;
		}
		m_videoParam.mfx.FrameInfo.Width = MSDK_ALIGN16(param.dwCX);
		m_videoParam.mfx.FrameInfo.Height = MSDK_ALIGN16(param.dwCY);
		m_videoParam.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		sts = m_videoENCODE->Query(&m_videoParam, &m_videoParam);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		if (!Allocate())
			return FALSE;
		sts = m_videoENCODE->Init(&m_videoParam);
		if (sts != MFX_ERR_NONE)
			return FALSE;
		sts = GetVideoParam();
		if (sts != MFX_ERR_NONE)
			return FALSE;
		m_taskPool.Reset(new Task[m_parameter.AsyncDepth]);
		memset(m_taskPool, 0, sizeof(Task) * m_parameter.AsyncDepth);
		for (INT i = 0; i < m_parameter.AsyncDepth; i++)
		{
			m_taskPool[i].mfxBS.MaxLength = m_parameter.mfx.BufferSizeInKB * 1000;
			m_taskPool[i].mfxBS.Data = new mfxU8[m_taskPool[i].mfxBS.MaxLength];
			m_taskPool[i].mfxBS.DataOffset = 0;
			m_taskPool[i].mfxBS.DataLength = 0;
			MSDK_CHECK_POINTER(m_taskPool[i].mfxBS.Data, MFX_ERR_MEMORY_ALLOC);
		}
		memset(&m_bitstream, 0, sizeof(mfxBitstream));
		m_bitstream.MaxLength = m_parameter.mfx.BufferSizeInKB * 1000;
		m_bitstream.Data = new mfxU8[m_bitstream.MaxLength];
		return TRUE;
	}

	mfxStatus QSVEncode::GetVideoParam()
	{
		ASSERT(m_videoENCODE);
		memset(&m_parameter, 0, sizeof(m_parameter));
		mfxExtCodingOptionSPSPPS opt;
		memset(&m_parameter, 0, sizeof(m_parameter));
		opt.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
		opt.Header.BufferSz = sizeof(mfxExtCodingOptionSPSPPS);
		static mfxExtBuffer* extendedBuffers[1];
		extendedBuffers[0] = (mfxExtBuffer*)& opt;
		m_parameter.ExtParam = extendedBuffers;
		m_parameter.NumExtParam = 1;
		opt.SPSBuffer = m_sps;
		opt.PPSBuffer = m_pps;
		opt.SPSBufSize = 100;
		opt.PPSBufSize = 100;
		mfxStatus sts = m_videoENCODE->GetVideoParam(&m_parameter);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_spsSize = opt.SPSBufSize;
		m_ppsSize = opt.PPSBufSize;
		return MFX_ERR_NONE;
	}
	mfxStatus QSVEncode::Allocate()
	{
		ASSERT(m_videoENCODE);
		mfxFrameAllocRequest request;
		memset(&request, 0, sizeof(request));
		mfxStatus sts = m_videoENCODE->QueryIOSurf(&m_videoParam, &request);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		request.Type |= WILL_WRITE;
		request.NumFrameSuggested += m_videoParam.AsyncDepth;
		sts = m_allocator.Alloc(m_allocator.pthis, &request, &m_response);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		m_surfaces.Reset(new mfxFrameSurface1 *[m_response.NumFrameActual]);
		MSDK_CHECK_POINTER(m_surfaces, MFX_ERR_MEMORY_ALLOC);
		for (INT i = 0; i < m_response.NumFrameActual; i++)
		{
			m_surfaces[i] = new mfxFrameSurface1();
			memset(reinterpret_cast<void*>(m_surfaces[i]), 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_surfaces[i]->Info), &(m_videoParam.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_surfaces[i]->Data.MemId = m_response.mids[i];
		}

	}
	BOOL QSVEncode::IntelExists()
	{
		TinyComPtr<IDXGIFactory1> factory;
		if (!SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
			return FALSE;
		UINT i = 0;
		TinyComPtr<IDXGIAdapter1> adapter;
		while (factory->EnumAdapters1(i++, &adapter) == S_OK)
		{
			DXGI_ADAPTER_DESC desc;
			if (!SUCCEEDED(adapter->GetDesc(&desc)))
				continue;
			if (desc.VendorId == 0x8086)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
}

