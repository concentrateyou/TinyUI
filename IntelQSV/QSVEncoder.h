#pragma once
#pragma once
#include "QSVAllocator.h"
using namespace TinyUI::Media;

namespace QSV
{
	class PartiallyLinearFNC
	{
		DISALLOW_COPY_AND_ASSIGN(PartiallyLinearFNC)
	private:
		mfxF64 *m_pX;
		mfxF64 *m_pY;
		mfxU32  m_nPoints;
		mfxU32  m_nAllocated;

	public:
		PartiallyLinearFNC();
		~PartiallyLinearFNC();
		void AddPair(mfxF64 x, mfxF64 y);
		mfxF64 GetAt(mfxF64);
	};

	class QSVEncoder
	{
		DISALLOW_COPY_AND_ASSIGN(QSVEncoder)
	public:
		QSVEncoder();
		virtual ~QSVEncoder();
	public:
		BOOL Open(const TinySize& src, const TinySize& dest, DWORD dwBitRate = 1000, DWORD dwFrameRate = 30);
		BOOL Encode(SampleTag& tag);
		void Close();
		BOOL GetSPSPPS(vector<mfxU8>& sps, vector<mfxU8>& pps);
		void LockSurface(mfxFrameSurface1* pSurface);
		void UnlockSurface(mfxFrameSurface1* pSurface);
		mfxBitstream	m_mfxResidial;
	private:
		mfxStatus Process(const BYTE* bits, LONG size);
		mfxStatus InitializeParam(const TinySize& src, const TinySize& dest, DWORD dwBitRate = 1000, DWORD dwFrameRate = 30);
		mfxStatus CreateAllocator(const TinySize& src, const TinySize& dest);
		mfxStatus AllocFrames();
		void DeleteFrames();
		void DeleteAllocator();
		INT GetFreeVPPSurfaceIndex();
		INT GetFreeVideoSurfaceIndex();
	private:
		mfxIMPL								m_mfxImpl;
		mfxVersion							m_mfxVersion;
		mfxEncodeCtrl						m_mfxEncodeCtrl;
		mfxExtCodingOption					m_mfxCodingOption;
		MFXVideoSession						m_mfxSession;
		mfxVideoParam						m_mfxVideoParam;
		mfxVideoParam						m_mfxVppVideoParam;
		mfxExtVPPDoNotUse					m_vppDoNotUse;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxFrameAllocResponse				m_mfxVPPResponse;
		std::vector<mfxExtBuffer*>			m_vppExtParams;
		std::vector<mfxExtBuffer*>			m_encExtParams;
		TinyScopedArray<mfxFrameSurface1*>	m_mfxSurfaces;
		TinyScopedArray<mfxFrameSurface1*>	m_mfxVPPSurfaces;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoVPP>			m_mfxVideoVPP;
		TinyScopedPtr<MFXVideoENCODE>		m_mfxVideoENCODE;
		volatile LONG						m_locks[256];
	};
}


