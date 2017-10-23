#include "stdafx.h"
#include "MSampleQueue.h"

namespace MShow
{
	MSampleQueue::MSampleQueue()
		:m_size(0)
	{
	}

	MSampleQueue::~MSampleQueue()
	{
	}
	LONGLONG MSampleQueue::GetMinimumTimeStamp()
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			Sample& value = m_list.GetAt(s);
			return static_cast<LONGLONG>(value.mediaTag.dwTime);
		}
		return -1;
	}
	LONGLONG MSampleQueue::GetMaximumTimeStamp()
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.Last();
			Sample& value = m_list.GetAt(s);
			return static_cast<LONGLONG>(value.mediaTag.dwTime);
		}
		return -1;
	}
	BOOL MSampleQueue::Push(Sample& sample)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.InsertLast(sample) != NULL)
		{
			m_size += sample.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MSampleQueue::Pop(Sample& sample)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			Sample& value = m_list.GetAt(s);
			memcpy(&sample, &value, sizeof(Sample));
			m_list.RemoveAt(s);
			m_size -= sample.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MSampleQueue::IsEmpty()
	{
		return m_list.IsEmpty();
	}
	LONG MSampleQueue::GetSize()
	{
		return m_size;
	}
	DWORD MSampleQueue::GetCount()
	{
		return m_list.GetSize();
	}

	void MSampleQueue::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		ITERATOR pos = m_list.First();
		while (pos != NULL)
		{
			Sample& sample = m_list.GetAt(pos);
			SAFE_DELETE_ARRAY(sample.bits);
			pos = m_list.Next(pos);
		}
		m_list.RemoveAll();
		m_size = 0;
	}
}
