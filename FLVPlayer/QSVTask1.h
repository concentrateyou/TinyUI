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

	class QSVTask : public TinyTaskBase
	{
	public:
		QSVTask();
		~QSVTask();
	public:
		BOOL	Initialize(HWND hWND);
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void		OnMessagePump();
		mfxStatus	OnInvoke();
		mfxStatus	OnVideo(const BYTE* bi, LONG si, LONGLONG timestamp, mfxFrameSurface1*& surface1);
		mfxStatus	OnVideo1(mfxBitstream& stream, mfxFrameSurface1*& surface1);
	private:
		FILE*								m_hFile;
		FLVReader							m_reader;
		DX11Graphics2D						m_graphics;
		DX11Image2D							m_video2D;

		mfxBitstream						m_residial;
		mfxSyncPoint						m_syncpDECODE;
		mfxSyncPoint						m_syncpVPP;
		mfxVideoParam						m_vppParam;
		mfxVideoParam						m_videoParam;
		MFXVideoSession						m_session;
		mfxFrameAllocator					m_allocator;
		mfxFrameAllocResponse				m_response;
		mfxFrameAllocResponse				m_reponses[2];
		TinyScopedPtr<MFXVideoDECODE>		m_videoDECODE;
		TinyScopedPtr<MFXVideoVPP>			m_videoVPP;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoISF;
		TinyScopedPtr<mfxFrameSurface1*>	m_videoOSF;
		TinyScopedArray<BYTE>				m_bits;
	};
}


