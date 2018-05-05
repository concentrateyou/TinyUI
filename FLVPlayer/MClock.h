#pragma once
#include "MShowCommon.h"

namespace FLVPlayer
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
		void SetClock(DOUBLE clock);
		DOUBLE GetClock();
		void SetBaseTime(LONGLONG baseTime);
		LONGLONG GetBaseTime();
		BOOL Sleep(DWORD dwMS = INFINITE);
		void Wake();
	private:
		DOUBLE		m_clock;
		LONGLONG	m_baseTime;
		TinyEvent	m_event;
	};
}

