#pragma once
#include "MShowCommon.h"

namespace TSPlayer
{
	/// <summary>
	/// ���ʱ���,��Ƶͬ����Ƶ
	/// </summary>
	class MClock
	{
		DISALLOW_COPY_AND_ASSIGN(MClock)
	public:
		MClock();
		~MClock();
		void SetClock(FLOAT clock);
		FLOAT GetClock();
		void SetBaseTime(LONGLONG baseTime);
		LONGLONG GetBaseTime();
		BOOL Sleep(DWORD dwMS = INFINITE);
		void Wake();
	private:
		FLOAT		m_clock;
		LONGLONG	m_baseTime;
		TinyEvent	m_event;
	};
}

