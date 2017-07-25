#pragma once
#pragma once
#include "QSVAllocator.h"
using namespace TinyUI::Media;
namespace QSV
{
	class QSVEncoder
	{
		DISALLOW_COPY_AND_ASSIGN(QSVEncoder)
	public:
		QSVEncoder();
		virtual ~QSVEncoder();
	public:
		BOOL Open(const TinySize& src, const TinySize& dest);
		void Close();
	private:
		mfxStatus InitializeParam(const TinySize& src, const TinySize& dest);
		mfxStatus CreateAllocator(const TinySize& src, const TinySize& dest);
		mfxStatus AllocFrames();
		void DeleteFrames();
		void DeleteAllocator();
	private:
		mfxIMPL								m_mfxImpl;
		mfxVersion							m_mfxVersion;
		MFXVideoSession						m_mfxSession;
		mfxVideoParam						m_mfxEncodeParam;
		mfxVideoParam						m_mfxVppParam;
		mfxExtVPPDoNotUse					m_vppDoNotUse;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxFrameAllocResponse				m_mfxVPPResponse;
		std::vector<mfxExtBuffer*>			m_vppExtParams;
		std::vector<mfxExtBuffer*>			m_EncExtParams;
		TinyScopedArray<mfxFrameSurface1>	m_mfxSurfaces;
		TinyScopedArray<mfxFrameSurface1>	m_mfxVPPSurfaces;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoVPP>			m_mfxVideoVPP;
		TinyScopedPtr<MFXVideoENCODE>		m_mfxVideoENCODE;
	};
}


