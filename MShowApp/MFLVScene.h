#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "FLVReader.h"
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"
#include "MElement.h"
using namespace Decode;

namespace MShow
{
	class MAudioTask;
	class MAudioRenderTask;
	class MVideoTask;
	class MVideoRenderTask;
	class MShowController;
	/// <summary>
	/// FLV³¡¾°
	/// </summary>
	class MFLVScene : public TinyTaskBase, public MElement
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVScene)
	public:
		MFLVScene(MShowController* pController);
		virtual ~MFLVScene();
		BOOL Initialize(DX2D& d2d, LPCSTR pzURL);
		BOOL SetVolume(LONG volume);
	public:
		BOOL Draw(DX2D& d2d) OVERRIDE;
		BOOL Submit() OVERRIDE;
		BOOL Close() OVERRIDE;
	public:
		Event<void(ID2D1Bitmap1*, INT, MElement*)>	EVENT_VIDEO;
	public:
		MPacketQueue&	GetAudioQueue();
		MPacketQueue&	GetVideoQueue();
		H264Decode*		GetH264();
		AACDecode*		GetAAC();
		FLV_SCRIPTDATA& GetScript();
	private:
		void OnMessagePump();
		void OnVideo(ID2D1Bitmap1* bitmap, INT delay);
	private:
		BOOL							m_bFI;
		LONGLONG						m_sample;
		TinyEvent						m_close;
		FLVReader						m_reader;
		MPacketQueue					m_audioQueue;
		MPacketQueue					m_videoQueue;
		MClock							m_clock;
		FLV_SCRIPTDATA					m_script;
		MShowController*				m_pController;
		TinyScopedPtr<H264Decode>		m_h264;
		TinyScopedPtr<AACDecode>		m_aac;
		TinyScopedPtr<MVideoTask>		m_videoTask;
		TinyScopedPtr<MAudioTask>		m_audioTask;
		TinyScopedPtr<MVideoRenderTask>	m_videoRenderTask;
		TinyScopedPtr<MAudioRenderTask>	m_audioRenderTask;
	};
}



