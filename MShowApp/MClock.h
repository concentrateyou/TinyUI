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
		void SetBaseTime(DWORD timestamp);
		void AddBaseTime(DWORD dwMS);
		LONGLONG GetBaseTime() const;
		LONGLONG GetBasePTS() const;
		void SetBasePTS(LONGLONG basePTS);
	private:
		DWORD		m_baseTime;
		LONGLONG	m_basePTS;
		TinyLock	m_lock;
	};
}

