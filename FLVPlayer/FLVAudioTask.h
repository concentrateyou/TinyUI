#pragma once
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include <concurrent_queue.h>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVAudioTask : public TinyTaskBase
	{
	public:
		FLVAudioTask();
		virtual ~FLVAudioTask();
		BOOL	Submit(BYTE* bits, LONG size, WORD wBitsPerSample);
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnAAC(BYTE* bits, LONG size, LPVOID ps);
	private:
		WAVEFORMATEX				m_waveFMT;
		TinyEvent					m_close;
		TinyScopedPtr<AACDecode>	m_aac;
	};

}


