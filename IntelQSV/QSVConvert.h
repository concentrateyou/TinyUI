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
		mfxStatus Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback);
		mfxStatus Convert(BYTE* data, LONG size);
		mfxStatus Close();
		QSVParam GetDefaultQSV(WORD wCX, WORD wCY, WORD wFPS);
	private:
		mfxStatus InitializeVPPParam(const QSVParam& param);
		mfxStatus Allocate(const QSVParam& param);
		void LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size);
	private:
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		mfxVideoParam						m_videoVPPParam;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_reponses[2];
		mfxSyncPoint						m_syncpVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceI;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceO;
		TinyScopedArray<BYTE>				m_bits;
		Callback<void(BYTE*, LONG)>			m_callback;
	};
}

