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
		if (m_list.InsertLast(tag) != NULL)
		{
			m_size += tag.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MPacketQueue::Pop(Decode::SampleTag& tag)
	{
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
	BOOL MPacketQueue::IsEmpty() const
	{
		return m_list.IsEmpty();
	}
	LONG MPacketQueue::GetSize() const
	{
		return m_size;
	}
	DWORD MPacketQueue::GetCount() const
	{
		return m_list.GetSize();
	}
	void MPacketQueue::RemoveAll()
	{
		ITERATOR pos = m_list.First();
		while (pos != NULL)
		{
			SampleTag& tag = m_list.GetAt(pos);
			SAFE_DELETE_ARRAY(tag.bits);
			pos = m_list.Next(pos);
		}
		m_list.RemoveAll();
	}
}
