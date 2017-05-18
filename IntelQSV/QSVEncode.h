#pragma once
#include "QSVCommon.h"

namespace QSV
{
	/// <summary>
	/// Intel QSV DX9
	/// https://github.com/sivabudh/intel-media-sdk-tutorials
	/// </summary>
	class QSVEncode
	{
		DISALLOW_COPY_AND_ASSIGN(QSVEncode)
	public:
		QSVEncode();
		virtual ~QSVEncode();
		mfxStatus Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback);
		mfxStatus Encode(BYTE* data, LONG size);
		mfxStatus Close();
		QSVParam GetDefaultQSV(WORD wCX, WORD wCY, WORD wKbps);
	private:
		void GetSPSPPS(mfxU8 **sps, mfxU8 **pps, mfxU16& spssize, mfxU16& ppssize);
		void LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size);
		mfxStatus InitializeEncodeParam(const QSVParam& param);
		mfxStatus InitializeVPPParam(const QSVParam& param);
		mfxStatus Allocate(const QSVParam& param);
		mfxStatus EncodeVPP(mfxFrameSurface1* surfaceIN, mfxFrameSurface1* surfaceOUT);
		mfxStatus EncodeVideo(mfxFrameSurface1* surfaceOUT);
	private:
		mfxBitstream						m_bitstream;
		mfxVideoParam						m_videoParam;
		mfxVideoParam						m_videoVPPParam;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_encodeReponse;
		mfxFrameAllocResponse				m_vppReponse;
		mfxExtVPPDoNotUse					m_vppDNU;
		mfxExtCodingOptionSPSPPS			m_spspps;
		mfxExtCodingOption					m_co;
		mfxSyncPoint						m_syncpVPP;
		mfxSyncPoint						m_syncpVideo;
		mfxU8								m_sps[100];
		mfxU8								m_pps[100];
		std::vector<mfxExtBuffer*>			m_encodeParams;
		std::vector<mfxExtBuffer*>			m_vppParams;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceEncode;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPP;
		Callback<void(BYTE*, LONG)>			m_callback;
	};
}



