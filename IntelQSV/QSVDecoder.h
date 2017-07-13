#pragma once
#include "QSVCommon.h"

namespace QSV
{
#define MAX_STREAM_SIZE (1024 * 1024)

	class QSVDecoder
	{
	public:
		QSVDecoder();
		~QSVDecoder();
		BOOL Open(const BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag, mfxFrameSurface1*& video);
		BOOL Close();
	private:
		mfxStatus Process(mfxBitstream& stream, mfxFrameSurface1*& video);
	private:
		mfxU16							m_sizeIN;
		mfxU16							m_sizeOUT;
		mfxSyncPoint					m_syncpDECODE;
		mfxSyncPoint					m_syncpVPP;
		MFXVideoSession					m_session;
		mfxBitstream					m_residial;
		mfxVideoParam					m_videoParam;
		mfxVideoParam					m_vppParam;
		mfxFrameAllocator				m_allocator;
		mfxFrameAllocResponse			m_response;
		mfxFrameAllocResponse			m_responseVPP;
		TinyScopedPtr<mfxFrameSurface1*>m_videoIN;
		TinyScopedPtr<mfxFrameSurface1*>m_videoOUT;
		TinyScopedPtr<MFXVideoDECODE>	m_videoDECODE;
		TinyScopedPtr<MFXVideoVPP>		m_videoVPP;
		TinyScopedArray<BYTE>			m_streamBits[2];
		TinyScopedArray<BYTE>			m_videoBits;
	};
}


