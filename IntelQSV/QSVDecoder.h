#pragma once
#include "QSVAllocator.h"

namespace QSV
{
#define MAX_STREAM_SIZE (1024 * 1024)

	struct SurfaceTag
	{
		mfxFrameSurface1*	surface1;
		LONGLONG			timestamp;
	};

	class QSVDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(QSVDecoder)
	public:
		QSVDecoder();
		virtual ~QSVDecoder();
		BOOL Open(const BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag, mfxFrameSurface1*& video);
		void Close();
		QSVAllocator* GetAllocator();
	private:
		mfxStatus Process(mfxBitstream& stream, LONGLONG& timestamp, mfxFrameSurface1*& video);
		mfxStatus InitializeVideoParam(const BYTE* bits, LONG size);
		mfxStatus CreateAllocator();
		mfxStatus AllocFrames();
		void DeleteFrames();
		void DeleteAllocator();
	private:
		TinyLinkList<SurfaceTag>			m_surfaceTags;
		TinyLinkList<Media::SampleTag>		m_tags;
		mfxIMPL								m_mfxImpl;
		mfxVersion							m_mfxVersion;
		MFXVideoSession						m_mfxSession;
		mfxBitstream						m_mfxResidial;
		mfxVideoParam						m_mfxVideoParam;
		mfxVideoParam						m_mfxVppVideoParam;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxFrameAllocResponse				m_mfxVPPResponse;
		mfxExtVPPDoNotUse					m_vppDoNotUse;
		vector<mfxExtBuffer*>				m_vppExtParams;
		TinyScopedArray<BYTE>				m_streamBits[2];
		TinyScopedArray<mfxFrameSurface1*>	m_mfxSurfaces;
		TinyScopedArray<mfxFrameSurface1*>	m_mfxVPPSurfaces;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoVPP>			m_mfxVideoVPP;
		TinyScopedPtr<MFXVideoDECODE>		m_mfxVideoDECODE;
	};
}



