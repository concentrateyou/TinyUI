#include "stdafx.h"
#include "PacketQueue.h"

PacketQueue::PacketQueue(TinyLock& lock)
	:m_size(0),
	m_lock(lock)
{

}

void PacketQueue::Push(SampleTag& tag)
{
	TinyAutoLock lock(m_lock);
	m_list.InsertLast(tag);
	m_size += tag.size;
}
SampleTag PacketQueue::Pop()
{
	TinyAutoLock lock(m_lock);
	SampleTag tag = { 0 };
	if (m_list.GetSize() > 0)
	{
		ITERATOR s = m_list.First();
		tag = m_list.GetAt(s);
		m_list.RemoveAt(s);
		m_size -= tag.size;
	}
	return tag;
}

INT PacketQueue::GetSize() const
{
	TinyAutoLock lock(m_lock);
	return m_size;
}

PacketQueue::~PacketQueue()
{

}
