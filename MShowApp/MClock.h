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
	private:
		LONG		m_baseTime;
		TinyLock	m_lock;
	};
}

