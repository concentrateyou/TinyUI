#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "common/common_utils.h"
#include <vector>
#include <dxgi.h>
using namespace TinyUI;

namespace QSV
{
	typedef struct tagQSVParam
	{
		WORD wCX;
		WORD wCY;
		WORD wFPS;
		WORD wKbps;
		WORD wMaxKbps;
		WORD wAccuracy;
		WORD wConvergence;
		WORD wQPI;
		WORD wQPB;
		WORD wQPP;
		WORD wRC;
		WORD wKeyPerSec;//每秒关键帧数
		WORD wbFrames;//b帧数
		WORD wAsyncDepth;
		WORD wTargetUsage;
	}QSVParam;
	/// <summary>
	/// Intel QSV 颜色空间转换VPP
	/// https://github.com/sivabudh/intel-media-sdk-tutorials
	/// </summary>
	class QSVColorSpaceConverter
	{
		DISALLOW_COPY_AND_ASSIGN(QSVColorSpaceConverter)
	public:
		QSVColorSpaceConverter();
		virtual ~QSVColorSpaceConverter();
		mfxStatus Initialize(const TinySize& i, mfxU32 iFourCC, const TinySize& o, mfxU32 oFourCC);
		mfxStatus Convert(BYTE* pI, DWORD sizeI, BYTE* pO, DWORD sizeO);
	private:
		mfxVideoParam		m_vppParams;
		MFXVideoSession		m_session;
		mfxFrameAllocator	m_mfxAllocator;
		mfxFrameSurface1**	m_pvvpSurfacesI;
		DWORD				m_surfacesI;
		mfxFrameSurface1**	m_pvvpSurfacesO;
		DWORD				m_surfacesO;
		TinyScopedPtr<MFXVideoVPP> m_mfxVPP;
	};
}



