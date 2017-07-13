#pragma once
#include "QSVCommon.h"

namespace QSV
{
	/// <summary>
	/// Intel QSV DX9
	/// https://github.com/sivabudh/intel-media-sdk-tutorials
	/// </summary>
	class QSVEncoder
	{
		DISALLOW_COPY_AND_ASSIGN(QSVEncoder)
	public:
		QSVEncoder();
		virtual ~QSVEncoder();
		mfxStatus Open(const QSVParam& param, Callback<void(BYTE*, LONG, INT)>&& callback);
		mfxStatus Encode(BYTE* data, LONG size);
		mfxStatus Close();
		QSVParam GetDefaultQSV(WORD wCX, WORD wCY, WORD wKbps);
		BOOL GetSPSPPS(vector<mfxU8>& sps, vector<mfxU8>& pps);
	private:
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
		mfxExtCodingOptionSPSPPS			m_spspps;
		mfxSyncPoint						m_syncpVPP;
		mfxSyncPoint						m_syncpVideo;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceEncode;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPP;
		Callback<void(BYTE*, LONG, INT)>	m_callback;
	};
}



