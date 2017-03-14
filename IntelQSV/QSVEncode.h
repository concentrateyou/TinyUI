#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "common/common_utils.h"
#include <dxgi.h>
using namespace TinyUI;

namespace QSV
{
	typedef struct tagQSVParam
	{
		WORD wTargetUsage;
		WORD wCX;
		WORD wCY;
		WORD wFPS;
		WORD wKbps;
		WORD wMaxKbps;
		WORD wICQQuality;
		WORD wAccuracy;
		WORD wConvergence;
		WORD wQPI;
		WORD wQPB;
		WORD wQPP;
		WORD wRC;
		WORD wKeyPerSec;//每秒关键帧数
		WORD wbFrames;//b帧数
		WORD wLookAheadDepth;
		WORD wAsyncDepth;
	}QSVParam;
	/// <summary>
	/// Intel QSV DX9
	/// https://github.com/sivabudh/intel-media-sdk-tutorials
	/// </summary>
	class QSVEncode
	{
	public:
		QSVEncode();
		~QSVEncode();
		mfxStatus Open(const QSVParam& param, Callback<void(BYTE*, LONG)>&& callback);
		mfxStatus Encode(BYTE* data, LONG size);
		mfxStatus Close();
		QSVParam GetDefaultQSV(WORD wCX, WORD wCY, WORD wKbps);
	private:
		void LoadRGBA(mfxFrameSurface1* surface, BYTE* data, LONG size);
		mfxStatus EncodeVPP(mfxFrameSurface1* surfaceIN, mfxFrameSurface1* surfaceOUT);
		mfxStatus EncodeVideo(mfxFrameSurface1* surfaceOUT);
	private:
		mfxU16					m_vppIN;
		mfxU16					m_vppOUT;
		mfxBitstream			m_bitstream;
		mfxExtVPPDoNotUse		m_vppDoNotUse;
		MFXVideoSession			m_session;
		mfxFrameAllocator		m_allocator;
		mfxVideoParam			m_videoParam;
		mfxVideoParam			m_videoVPPParam;
		mfxFrameAllocResponse	m_videoVPPINReponse;
		mfxFrameAllocResponse	m_videoVPPOUTReponse;
		mfxSyncPoint			m_syncpVPP;
		mfxSyncPoint			m_syncpVideo;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPPIN;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPPOUT;
		Callback<void(BYTE*, LONG)>			m_callback;
	};

}



