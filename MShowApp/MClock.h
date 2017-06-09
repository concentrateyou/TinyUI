#pragma once
#include "MShowCommon.h"

namespace MShow
{
	/// <summary>
	/// ���ʱ���
	/// </summary>
	class MClock
	{
		DISALLOW_COPY_AND_ASSIGN(MClock)
	public:
		MClock();
		~MClock();
		void SetBaseTime(LONG timestamp);
		LONG GetBaseTime() const;
	private:
		LONG		m_timestamp;
	};
}

