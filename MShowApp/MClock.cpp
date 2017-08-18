#include "stdafx.h"
#include "MClock.h"

namespace MShow
{
	MClock::MClock()
		:m_baseTime(-1),
		m_basePTS(-1),
		m_videoPTS(-1),
		m_audioPTS(-1),
		m_audioDelay(0),
		m_videoDelay(0)
	{
	}

	MClock::~MClock()
	{
	}
	void MClock::SetBaseTime(LONGLONG baseTime)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime = baseTime;
	}
	void MClock::AddBaseTime(LONGLONG dwMS)
	{
		if (dwMS > 0)
		{
			TinyAutoLock lock(m_lock);
			m_baseTime += dwMS;
		}
	}
	LONGLONG MClock::GetBaseTime() const
	{
		return m_baseTime;
	}
	LONGLONG MClock::GetBasePTS() const
	{
		return m_basePTS;
	}
	void MClock::SetBasePTS(LONGLONG basePTS)
	{
		m_basePTS = basePTS;
	}
	void MClock::SetVideoPTS(LONGLONG offsetPTS)
	{
		m_videoPTS = offsetPTS;
	}
	LONGLONG MClock::GetVideoPTS() const
	{
		return m_videoPTS;
	}
	void MClock::SetAudioPTS(LONGLONG audioPTS)
	{
		m_audioPTS = audioPTS;
	}
	LONGLONG MClock::GetAudioPTS() const
	{
		return m_audioPTS;
	}
	void MClock::SetAudioDelay(LONGLONG delay)
	{
		m_audioDelay = delay;
	}
	LONGLONG MClock::GetAudioDelay() const
	{
		return m_audioDelay;
	}
	void MClock::SetVideoDelay(LONGLONG delay)
	{
		m_videoDelay = delay;
	}
	LONGLONG MClock::GetVideoDelay() const
	{
		return m_videoDelay;
	}
}

