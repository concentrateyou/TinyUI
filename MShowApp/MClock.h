#pragma once
#include "MShowCommon.h"

namespace MShow
{
	/// <summary>
	/// 相对时间戳
	/// </summary>
	class MClock
	{
		DISALLOW_COPY_AND_ASSIGN(MClock)
	public:
		MClock();
		~MClock();
		void SetBaseTime(LONGLONG timestamp);
		void AddBaseTime(LONGLONG dwMS);
		LONGLONG GetBaseTime() const;
		LONGLONG GetBasePTS() const;
		void SetBasePTS(LONGLONG basePTS);
		void SetVideoPTS(LONGLONG videoPTS);
		void SetAudioDelay(LONGLONG delay);
		LONGLONG GetAudioDelay() const;
		void SetVideoDelay(LONGLONG delay);
		LONGLONG GetVideoDelay() const;
		LONGLONG GetVideoPTS() const;
		void SetAudioPTS(LONGLONG audioPTS);
		LONGLONG GetAudioPTS() const;
	private:
		LONGLONG	m_baseTime;
		LONGLONG	m_basePTS;
		LONGLONG	m_videoPTS;
		LONGLONG	m_audioPTS;
		LONGLONG	m_audioDelay;
		LONGLONG	m_videoDelay;
		TinyLock	m_lock;
	};
}

