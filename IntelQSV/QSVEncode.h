#pragma once
#include "Common/TinyCommon.h"
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
	/// DX11
	/// https://github.com/sivabudh/intel-media-sdk-tutorials
	/// </summary>
	class QSVEncode
	{
	public:
		QSVEncode();
		~QSVEncode();
		mfxStatus Open(const QSVParam& param);
		mfxStatus Encode(BYTE* data, LONG size);
		mfxStatus Close();
	protected:
		MFXVideoSession			m_session;
		mfxFrameAllocator		m_allocator;
		mfxVideoParam			m_videoParam;
		mfxVideoParam			m_videoVPPParam;
		mfxFrameAllocRequest	m_videoRequest;
		mfxU16					m_vppIN;
		mfxU16					m_vppOUT;
		mfxBitstream			m_bitstream;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPPIN;
		TinyScopedPtr<mfxFrameSurface1*>	m_surfaceVPPOUT;
	};

}



