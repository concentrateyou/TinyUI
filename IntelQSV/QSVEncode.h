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
		QSVParam GetDefaultQSV(WORD wCX, WORD wCY);
		BOOL Open(const QSVParam& param);
		mfxStatus Encode(UINT64 ts, BYTE *pDataY, BYTE *pDataUV, UINT32 strideY, UINT32 strideUV, mfxBitstream **pBS);
		BOOL Close();
	private:
		mfxStatus LoadNV12(mfxFrameSurface1 *pSurface, BYTE *pDataY, BYTE* pDataUV, UINT32 strideY, UINT32 strideUV);
		mfxStatus Allocate();
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
		TinyScopedArray<Task>				m_tasks;
		TinyScopedArray<mfxFrameSurface1*>	m_surfaces;
		TinyScopedPtr<MFXVideoENCODE>		m_videoENCODE;
	};
}



