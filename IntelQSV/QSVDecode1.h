#pragma once
#include "QSVCommon.h"

namespace QSV
{
	class QSVDecode
	{
		DISALLOW_COPY_AND_ASSIGN(QSVDecode)
	public:
		QSVDecode();
		virtual ~QSVDecode();
		mfxStatus Initialize();
		mfxStatus Open(BYTE* bits, LONG size);
		mfxStatus Decode(BYTE* bits, LONG size, LONGLONG timestamp);
		mfxStatus Close();
	private:
		mfxBitstream						m_bitstream;
		mfxFrameSurface1*					m_currentSF;
		mfxSyncPoint						m_syncDECODE;
		mfxSyncPoint						m_syncpVPP;
		mfxVideoParam						m_vppParam;
		mfxVideoParam						m_videoParam;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_response;
		mfxFrameAllocResponse				m_reponses[2];
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoISF;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoOSF;
		FILE*								m_hFile;
	};
}


