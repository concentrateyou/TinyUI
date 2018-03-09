#pragma once
#include "MShowCommon.h"

namespace TSPlayer
{
	/// <summary>
	/// 相对时间戳,视频同步音频
	/// </summary>
	class MClock
	{
		DISALLOW_COPY_AND_ASSIGN(MClock)
	public:
		MClock();
		~MClock();
		void SetAudioClock(LONGLONG audioClock);
		LONGLONG GetAudioClock();
		void SetBaseTime(LONGLONG baseTime);
		LONGLONG GetBaseTime();
		BOOL Sleep(DWORD dwMS = INFINITE);
		void Wake();
	private:
		LONGLONG	m_baseTime;
		LONGLONG	m_audioClock;
		TinyEvent	m_event;
	};
}

