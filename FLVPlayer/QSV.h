#pragma once
#include "Media/TinySoundPlayer.h"
#include "IO/TinyTaskBase.h"
#include "Common/TinyTime.h"
#include "FLVReader.h"
#include "PacketQueue.h"
#include "QSVDecode.h"
#include "DX11Graphics2D.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace DXFramework;

namespace FLVPlayer
{
#define MAX_STREAM_SIZE (1024 * 1024)

	class QSV
	{
	public:
		QSV();
		~QSV();
		BOOL Open(const BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag, mfxFrameSurface1*& surface1);
		BOOL Close();
		BOOL Lock(mfxFrameSurface1* surface1);
		BOOL Unlock(mfxFrameSurface1* surface1);
	private:
		mfxStatus Process(mfxBitstream& stream, mfxFrameSurface1*& surface1);
	private:
		mfxSyncPoint					m_syncp;
		MFXVideoSession					m_session;
		mfxBitstream					m_residial;
		mfxVideoParam					m_videoParam;
		mfxFrameAllocator				m_allocator;
		mfxFrameAllocResponse			m_response;
		TinyScopedPtr<mfxFrameSurface1*>m_surfaces;
		TinyScopedPtr<MFXVideoDECODE>	m_videoDECODE;
		TinyScopedArray<BYTE>			m_bits[2];
	};
}


