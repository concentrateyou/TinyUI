#pragma once
#include "QSVCommon.h"
#include "QSVD3D.h"
#include "QSVAllocator.h"
using namespace TinyUI;

namespace QSV
{
	/// <summary>
	/// QSV Decoder
	/// </summary>
	class QuickSyncDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(QuickSyncDecoder)
	public:
		QuickSyncDecoder(const QSVConfig& config);
		~QuickSyncDecoder();
	public:
		mfxStatus Initialize(mfxVideoParam* pVideoParams, mfxU32 sPitch);
		mfxStatus Decode(mfxBitstream* pI, mfxFrameSurface1*& pO);
		void SetAssists(mfxU16 count);
	private:
		mfxStatus InitializeSession(mfxIMPL impl);
		mfxStatus Reset(mfxVideoParam* pVideoParams, mfxU32 sPitch);
		mfxStatus CreateAllocator();
		mfxStatus DestoryAllocator();
		mfxStatus AllocFrames(mfxVideoParam* pVideoParams, mfxU32 sPitch);
		mfxStatus FreeFrames();
	private:
		BOOL								m_bAcceleration;
		BOOL								m_bUseD3DAlloc;
		BOOL								m_bUseD3D11Alloc;
		mfxU16								m_assists;//¸¨ÖúÖ¡¸öÊý
		mfxU16								m_requests;
		mfxVersion							m_mfxVersion;
		mfxIMPL								m_mfxIMPL;
		mfxVideoParam*						m_pVideoParams;
		mfxFrameSurface1*					m_pFrameSurfaces;
		mfxFrameAllocResponse				m_allocResponse;
		QSVConfig							m_config;
		TinyLinkList<mfxFrameSurface1*>		m_queue;
		TinyScopedArray<mfxFrameSurface1>	m_surfaces;
		TinyScopedPtr<QSVD3D>				m_qsvd3d;
		TinyScopedPtr<MFXVideoSession>		m_mfxVideoSession;
		TinyScopedPtr<MFXVideoDECODE>		m_mfxVideoDECODE;
		TinyScopedPtr<QSVAllocator>			m_allocator;
		volatile LONG						m_locks[MSDK_MAX_SURFACES];
		TinyLock							m_lock;
	};
}


