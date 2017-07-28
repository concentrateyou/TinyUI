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
		BOOL Encode(SampleTag& tag, BYTE*& bits, LONG& size, MediaTag& mediaTag);
		void Close();
		vector<mfxU8>& GetSPS();
		vector<mfxU8>& GetPPS();
	private:
		mfxStatus Process(SampleTag& tag, BYTE*& bo, LONG& so, MediaTag& mediaTag);
		mfxStatus InitializeParam(const TinySize& src, const TinySize& dest, DWORD dwBitRate = 3231, DWORD dwFrameRate = 30);
		mfxStatus CreateAllocator(const TinySize& src, const TinySize& dest);
		mfxStatus AllocFrames();
		void DeleteFrames();
		void DeleteAllocator();
		INT GetFreeVPPSurfaceIndex();
		INT GetFreeVideoSurfaceIndex();
		void LoadRGB32(mfxFrameSurface1* pIN, const BYTE* bits, LONG size);
	private:
		DWORD								m_dwINC;
		mfxIMPL								m_mfxImpl;
		mfxVersion							m_mfxVersion;
		mfxBitstream						m_mfxResidial;
		mfxEncodeCtrl						m_mfxEncodeCtrl;
		MFXVideoSession						m_mfxSession;
		mfxVideoParam						m_mfxVideoParam;
		mfxVideoParam						m_mfxVppVideoParam;
		mfxExtCodingOption					m_mfxCodingOption;
		mfxExtCodingOptionSPSPPS			m_mfxCodingOptionSPSPPS;
		mfxExtVPPDoNotUse					m_vppDoNotUse;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxFrameAllocResponse				m_mfxVPPResponse;
		TinyScopedArray<BYTE>				m_streamBits[2];
		std::vector<mfxU8>					m_mfxSPS;
		std::vector<mfxU8>					m_mfxPPS;
		std::vector<mfxExtBuffer*>			m_vppExtParams;
		std::vector<mfxExtBuffer*>			m_encExtParams;
		TinyScopedArray<mfxFrameSurface1*>	m_mfxSurfaces;
		TinyScopedArray<mfxFrameSurface1*>	m_mfxVPPSurfaces;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoVPP>			m_mfxVideoVPP;
		TinyScopedPtr<MFXVideoENCODE>		m_mfxVideoENCODE;
	};
}


