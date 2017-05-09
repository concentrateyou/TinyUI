#pragma once
#include "FLVStream.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include "FLVAudioTask.h"
#include "FLVVideoTask.h"
#include <queue>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVDecodeTask : public TinyTaskBase
	{
	public:
		FLVDecodeTask(HWND hWND);
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
		TinyLock					m_videoLock;
		TinyLock					m_audioLock;
		TinySize					m_size;
		FLVStream					m_parse;
		TinyScopedPtr<FLVAudioTask>	m_audioTask;
		TinyScopedPtr<FLVVideoTask>	m_videoTask;
	};
}


