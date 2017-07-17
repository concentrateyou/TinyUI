#pragma once
#include "QSVD3D.h"

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
		BOOL Initialize(mfxFrameAllocator& allocator);
	private:
		mfxStatus Process(mfxBitstream& stream, Media::SampleTag& tag, mfxFrameSurface1*& video);
		static mfxStatus Alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response);
		static mfxStatus Lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);
		static mfxStatus Unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);
		static mfxStatus GetHDL(mfxHDL pthis, mfxMemId mid, mfxHDL* handle);
		static mfxStatus Free(mfxHDL pthis, mfxFrameAllocResponse* response);
	private:
		mfxU16								m_sizeIN;
		mfxU16								m_sizeOUT;
		mfxSyncPoint						m_syncpDECODE;
		mfxSyncPoint						m_syncpVPP;
		MFXVideoSession						m_session;
		mfxBitstream						m_residial;
		mfxVideoParam						m_videoParam;
		mfxVideoParam						m_vppParam;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_response;
		mfxFrameAllocResponse				m_responseVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoIN;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoOUT;
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedArray<BYTE>				m_streamBits[2];
		TinyScopedArray<BYTE>				m_videoBits;
		QSVD3D9								m_d3d9;
	};
}


