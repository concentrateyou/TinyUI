#pragma once
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include "FLVAudioTask.h"
#include "FLVVideoTask.h"
#include <deque>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVDecodeTask : public TinyTaskBase
	{
	public:
		FLVDecodeTask();
		~FLVDecodeTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnScript(FLV_SCRIPTDATA* val);
		void	OnAudio(BYTE* bits, LONG size, FLV_PACKET* packet);
		void	OnVideo(BYTE* bits, LONG size, FLV_PACKET* packet);
	private:
		TinyScopedPtr<Delegate<void(FLV_SCRIPTDATA*)>>			m_onScript;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PACKET*)>>	m_onAudio;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PACKET*)>>	m_onVideo;
	private:
		DWORD						m_rate;
		TinySize					m_size;
		FLVParse					m_parse;
		TinyEvent					m_wait;
		TinyLock					m_lock;
		std::deque<AVPacket>		m_packets;
		TinyScopedPtr<FLVAudioTask>	m_audioTask;
		TinyScopedPtr<FLVVideoTask>	m_videoTask;
	};
}


