#include "stdafx.h"
#include "MPacketQueue.h"

namespace MShow
{
	MPacketQueue::MPacketQueue(TinyLock& lock)
		:m_lock(lock)
	{
	}

	MPacketQueue::~MPacketQueue()
	{
	}
	void MPacketQueue::Push(Decode::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		m_list.InsertLast(tag);
		m_size += tag.size;
	}
	Decode::SampleTag MPacketQueue::Pop()
	{
		TinyAutoLock lock(m_lock);
		Decode::SampleTag tag = { 0 };
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			tag = m_list.GetAt(s);
			m_list.RemoveAt(s);
			m_size -= tag.size;
		}
		return tag;
	}
	BOOL MPacketQueue::IsEmpty() const
	{

	}
	INT MPacketQueue::GetSize() const
	{

	}
	INT MPacketQueue::GetCount() const
	{

	}
}
