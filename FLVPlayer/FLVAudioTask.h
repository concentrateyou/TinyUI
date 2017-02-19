#pragma once
#include "Media/TinySoundPlayer.h"
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "Common/TinyTime.h"
#include "IO/TinyTaskBase.h"
#include <queue>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace FLVPlayer
{

	class FLVDecodeTask;

	class FLVAudioTask : public TinyTaskBase
	{
	public:
		FLVAudioTask(FLVDecodeTask* pTask, TinyLock& lock, HWND hWND);
		virtual ~FLVAudioTask();
		BOOL	Submit(BYTE* bits, LONG size, WORD wBitsPerSample);
		BOOL	Close(DWORD dwMs) OVERRIDE;
		void	Push(const SampleTag& tag);
	private:
		void	OnMessagePump();
		BOOL	OnProcessTag(LONG& pts);
		void	OnRender(BYTE* bits, LONG size);
	private:
		LONG						m_currentPTS;
		HWND						m_hWND;
		TinyEvent					m_close;
		TinyEvent					m_wait;
		TinyLock&					m_lock;
		FLVDecodeTask*				m_pTask;
		TinyPerformanceTimer		m_timer;
		std::queue<SampleTag>		m_queue;
		TinyScopedPtr<AACDecode>	m_aac;
		TinyBufferArray<BYTE>		m_audios;
		TinySoundPlayer				m_player;
		TinyEvent					m_event[2];
	};

}


