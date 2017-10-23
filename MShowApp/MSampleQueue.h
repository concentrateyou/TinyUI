#pragma once
#include "MShowCommon.h"

namespace MShow
{
	/// <summary>
	/// 编码队列数据
	/// </summary>
	class MSampleQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MSampleQueue)
	public:
		MSampleQueue();
		~MSampleQueue();
		INT64 GetMinimumTimeStamp();
		INT64 GetMaximumTimeStamp();
		BOOL Push(Sample& sample);
		BOOL Pop(Sample& sample);
		BOOL IsEmpty();
		LONG GetSize();
		DWORD GetCount();
		void RemoveAll();
	private:
		LONG					m_size;
		TinyLock				m_lock;
		TinyLinkList<Sample>	m_list;
	};
}



