#pragma once
#include "QSVCommon.h"

namespace QSV
{
	/// <summary>
	/// RGBA ---> I420
	/// </summary>
	class QSVConvert
	{
		DISALLOW_COPY_AND_ASSIGN(QSVConvert)
	public:
		QSVConvert();
		~QSVConvert();
	public:
		mfxStatus Open(const TinySize& src, DWORD dwFPSSRC, const TinySize& dst, DWORD dwFPSDST, Callback<void(BYTE*, LONG)>&& callback);
		mfxStatus Convert(BYTE* data, LONG size);
		mfxStatus Close();
	private:
		mfxStatus InitializeVPPParam(const TinySize& src, DWORD dwFPSSRC, const TinySize& dst, DWORD dwFPSDST);
		mfxStatus Allocate();
		void LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size);
	private:
		TinySize							m_dstSize;
		TinySize							m_srcSize;
		mfxVideoParam						m_videoVPPParam;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_reponses[2];
		mfxSyncPoint						m_syncpVPP;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceI;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceO;
		TinyScopedArray<BYTE>				m_bits;
		Callback<void(BYTE*, LONG)>			m_callback;
	};
}

