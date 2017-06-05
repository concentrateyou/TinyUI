#pragma once
#include "DX2D.h"
#include "RTMPReader.h"
#include "Media/TinySoundPlayer.h"
#include "H264Decode.h"
#include "AACDecode.h"
using namespace Decode;
using namespace Media;
using namespace DXFramework;

namespace MShow
{
	class MRenderTask : public TinyTaskBase
	{
	public:
		MRenderTask();
		~MRenderTask();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		BOOL Close(DWORD dwMS) OVERRIDE;
		BOOL Submit(LPCSTR pzURL);
	private:
		void OnMessagePump();
		void OnScript(FLV_SCRIPTDATA*);
		void OnAudio(BYTE*, LONG, FLV_PACKET*);
		void OnVideo(BYTE*, LONG, FLV_PACKET*);
	private:
		HWND			m_hWND;
		TinySize		m_size;
		DX2D			m_dx;
		RTMPReader		m_reader;
		TinyEvent		m_close;
		TinySoundPlayer m_splayer;
		TinyScopedPtr < Delegate<void(BYTE*, LONG, FLV_PACKET*)> >	m_onAudio;
		TinyScopedPtr < Delegate<void(BYTE*, LONG, FLV_PACKET*)> >	m_onVideo;
		TinyScopedPtr < Delegate<void(FLV_SCRIPTDATA*)> >			m_onScript;
		FLV_SCRIPTDATA	m_script;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
		BOOL						m_bInitializeA;
		BOOL						m_bInitializeV;
		TinyLock					m_lock;
		PacketQueue					m_videoQueue;
		
	};
}



