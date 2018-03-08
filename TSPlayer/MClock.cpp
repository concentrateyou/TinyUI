#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_audioPTS(INVALID_TIME)
	{
		m_signal.CreateEvent();
	}

	MClock::~MClock()
	{
	}
	void MClock::SetAudioPTS(LONGLONG audioPTS)
	{
		TRACE("SetAudioPTS:%lld\n", audioPTS);
		m_audioPTS = audioPTS;
	}
	LONGLONG MClock::GetAudioPTS()
	{
		return m_audioPTS;
	}

	BOOL MClock::LockVideo(DWORD dwMS)
	{
		return m_signal.Lock(dwMS);
	}
	void MClock::UnlockVideo()
	{
		m_signal.SetEvent();
	}
}

