#pragma once
#include "QSVAllocator.h"

namespace QSV
{
#define MAX_ALLOC_SIZE (1024 * 1024)

	class QSVDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(QSVDecoder)
	public:
		QSVDecoder();
		virtual ~QSVDecoder();
		BOOL Open(const BYTE* bits, LONG size);
	private:
		mfxStatus InitializeMFXParam(const BYTE* bits, LONG size);
		mfxStatus CreateAllocator();
		mfxStatus AllocFrames();
	private:
		mfxIMPL								m_mfxImpl;
		mfxVersion							m_mfxVersion;
		MFXVideoSession						m_mfxSession;
		mfxBitstream						m_mfxBitstream;
		mfxVideoParam						m_mfxVideoParam;
		mfxVideoParam						m_mfxVppVideoParam;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxFrameAllocResponse				m_mfxVppResponse;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoVPP>			m_mfxVideoVPP;
		TinyScopedPtr<MFXVideoDECODE>		m_mfxVideoDECODE;
	};
}



