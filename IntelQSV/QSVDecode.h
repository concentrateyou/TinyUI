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
		mfxFrameSurface1*					m_currentSF;
		mfxSyncPoint						m_syncPoint;
		mfxVideoParam						m_videoParams;
		mfxVideoParam						m_vppParams;
		mfxBitstream						m_bitstream;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocRequest				m_request;
		mfxFrameAllocResponse				m_response;
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		TinyScopedPtr<mfxFrameSurface1*>	m_decodeSF;
		FILE*								m_hFile;
	};
}


