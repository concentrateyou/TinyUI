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
		mfxStatus Open(BYTE* bits, LONG size, Callback<void(BYTE*, LONG, INT)>&& callback);
		mfxStatus Decode(BYTE* data, LONG size);
		mfxStatus Close();
	private:
		mfxBitstream						m_bitstream;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocRequest				m_allocRequest;
		mfxFrameAllocResponse				m_allocResponse;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaces;
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		Callback<void(BYTE*, LONG, INT)>	m_callback;
	};
}


