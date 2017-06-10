#pragma once
#include "MShowCommon.h"

namespace MShow
{
#define MAX_QUEUE_SIZE (1 * 1024 * 1024)
#define MAX_AUDIO_QUEUE_SIZE (48 * 1024)
#define MAX_VIDEO_QUEUE_SIZE (6 * 1024 * 1024)
	/// <summary>
	/// 数据队列
	/// </summary>
	class MPacketQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MPacketQueue)
	public:
		MPacketQueue();
		~MPacketQueue();
		BOOL Push(SampleTag& tag);
		BOOL Pop(SampleTag& tag);
		BOOL IsEmpty() const;
		LONG GetSize() const;
		DWORD GetCount() const;
		void RemoveAll();
	private:
		LONG					m_size;
		TinyLock				m_lock;
		TinyLinkList<SampleTag>	m_list;
	};
}



