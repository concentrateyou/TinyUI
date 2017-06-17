#include "stdafx.h"
#include "MPacketQueue.h"

namespace MShow
{
	MPacketQueue::MPacketQueue()
	{
	}

	MPacketQueue::~MPacketQueue()
	{
	}
	BOOL MPacketQueue::Push(Decode::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.InsertLast(tag) != NULL)
		{
			m_size += tag.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MPacketQueue::Pop(Decode::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			Decode::SampleTag& sampleTag = m_list.GetAt(s);
			memcpy(&tag, &sampleTag, sizeof(Decode::SampleTag));
			m_list.RemoveAt(s);
			m_size -= tag.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MPacketQueue::IsEmpty()
	{
		return m_list.IsEmpty();
	}
	LONG MPacketQueue::GetSize()
	{
		return m_size;
	}
	DWORD MPacketQueue::GetCount()
	{
		return m_list.GetSize();
	}

	void MPacketQueue::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		ITERATOR pos = m_list.First();
		while (pos != NULL)
		{
			SampleTag& tag = m_list.GetAt(pos);
			SAFE_DELETE_ARRAY(tag.bits);
			pos = m_list.Next(pos);
		}
		m_list.RemoveAll();
		m_size = 0;
	}
}
