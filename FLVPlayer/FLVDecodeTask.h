#pragma once
#include "FLVParse.h"
#include "IO/TinyTaskBase.h"
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
		void	OnH264(BYTE* bits, LONG size, LPVOID ps);
		void	OnAAC(BYTE* bits, LONG size, LPVOID ps);
	private:
		
		TinyScopedPtr<Delegate<void(FLV_SCRIPTDATA*)>>			m_onScript;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PACKET*)>>	m_onAudio;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PACKET*)>>	m_onVideo;
	private:
		DWORD						m_rate;
		TinySize					m_size;
		FLVParse					m_parse;
		WAVEFORMATEX				m_waveFMT;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
	};
}


