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
		mfxStatus Initialize(Callback<void(BYTE*, LONG, INT)>&& callback);
		mfxStatus Open(BYTE* bits, LONG size);
		mfxStatus Decode(BYTE* bits, LONG size, LONGLONG timestamp);
		mfxStatus Close();
	private:
		mfxSyncPoint						m_syncp;
		mfxVideoParam						m_videoParams;
		mfxBitstream						m_bitstream;
		MFXVideoSession						m_session;
		mfxFrameSurface1*					m_surface;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocRequest				m_allocRequest;
		mfxFrameAllocResponse				m_allocResponse;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaces;
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		Callback<void(BYTE*, LONG, INT)>	m_callback;
	};
}


