#pragma once
#include "QSVCommon.h"
#include "QSVD3D.h"
#include "QSVAllocator.h"
using namespace TinyUI;
using namespace TinyUI::Media;

namespace QSV
{
	/// <summary>
	/// QSV Decoder
	/// </summary>
	class QuickSyncDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(QuickSyncDecoder)
	public:
		QuickSyncDecoder();
		~QuickSyncDecoder();
	public:
		BOOL Open(BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag, mfxFrameSurface1*& video);
		void Close();
		QSVAllocator* GetAllocator();
		void LockSurface(mfxFrameSurface1* pSurface);
		void UnlockSurface(mfxFrameSurface1* pSurface);
	private:
		mfxStatus Process(mfxBitstream& stream, mfxFrameSurface1*& video);
		mfxStatus InitializeVideoParams(BYTE* bits, LONG size);
		mfxStatus CreateAllocator();
		mfxStatus AllocFrames();
		void FreeFrames();
		void DestoryAllocator();
		INT GetFreeVideoSurfaceIndex();
	private:
		BOOL								m_bHDW;
		BOOL								m_bAllowD3D;
		BOOL								m_bAllowD3D11;
		mfxU16								m_assist;
		mfxIMPL								m_mfxIMPL;
		mfxVersion							m_mfxVersion;
		mfxFrameAllocResponse				m_mfxResponse;
		mfxVideoParam						m_mfxVideoParam;
		mfxBitstream						m_mfxResidial;
		TinyLinkList<SampleTag>				m_tags;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		TinyScopedPtr<MFXVideoSession>		m_mfxVideoSession;
		TinyScopedPtr<MFXVideoDECODE>		m_mfxVideoDECODE;
		TinyLinkList<mfxFrameSurface1*>		m_outputs;
		TinyScopedArray<mfxFrameSurface1>	m_mfxSurfaces;
		TinyScopedArray<BYTE>				m_streamBits[2];
		volatile LONG						m_locks[MSDK_MAX_SURFACES];
	};
}


