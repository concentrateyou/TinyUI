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
		void SetBaseTime(LONG timestamp);
		void AddBaseTime(DWORD dwMS);
		LONG GetBaseTime() const;
		LONG GetBasePTS() const;
		void SetBasePTS(LONG basePTS);
	private:
		LONG		m_baseTime;
		LONG		m_basePTS;
		TinyLock	m_lock;
	};
}

