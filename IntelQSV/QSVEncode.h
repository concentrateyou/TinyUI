#pragma once
#include "Common/TinyCommon.h"
#include "common/common_utils.h"
#include <dxgi.h>
using namespace TinyUI;

namespace QSV
{
	typedef struct tagQSVParam
	{
		DWORD dwCX;
		DWORD dwCY;
		DWORD dwFPS;
		DWORD dwKbps;
		DWORD dwMaxKbps;
		DWORD dwICQQuality;
		DWORD dwAccuracy;
		DWORD dwConvergence;
		DWORD dwQPI;
		DWORD dwQPB;
		DWORD dwQPP;
		DWORD dwRC;
		DWORD dwGopPicSize;
		DWORD dwGopRefDist;
		DWORD dwLookAheadDepth;
		DWORD dwAsyncDepth;
	}QSVParam;
	//https://github.com/sivabudh/intel-media-sdk-tutorials
	class QSVEncode
	{
	public:
		QSVEncode();
		~QSVEncode();
		BOOL Initialize(const QSVParam& param);
	private:
		BOOL AllocateSurfaces();
		mfxStatus GetVideoParam();
		BOOL IntelExists();
	private:
		mfxVideoParam           m_parameter;
		mfxVideoParam			m_videoParam;
		MFXVideoSession			m_session;
		mfxFrameAllocator		m_allocator;
		mfxFrameAllocResponse   m_response;
		mfxU8                   m_sps[100];
		mfxU8                   m_pps[100];
		mfxU16                  m_spsSize;
		mfxU16                  m_ppsSize;
		mfxBitstream			m_bitstream;
		mfxExtCodingOption2     m_co2;
		mfxExtCodingOption      m_co;
		TinyScopedArray<Task>				m_taskPool;
		TinyScopedArray<mfxFrameSurface1*>	m_surfaces;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
	};
}



