#include "stdafx.h"
#include "MAudioQueue.h"

namespace MShow
{
	MAudioQueue::MAudioQueue()
		:m_size(0)
	{

	}

	MAudioQueue::~MAudioQueue()
	{

	}

	UINT MAudioQueue::GetAllocSize()
	{
		return m_alloc.GetAllocSize();
	}

	void MAudioQueue::Initialize(UINT count, UINT size)
	{
		m_alloc.Initialize(count, size);
	}

	void* MAudioQueue::Alloc()
	{
		return m_alloc.Alloc();
	}

	void MAudioQueue::Free(void* p)
	{
		m_alloc.Free(p);
	}

	BOOL MAudioQueue::Push(AUDIO_SAMPLE& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.InsertLast(tag) != NULL)
		{
			m_size += tag.size;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MAudioQueue::Pop(AUDIO_SAMPLE& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			AUDIO_SAMPLE& sampleTag = m_list.GetAt(s);
			memcpy(&tag, &sampleTag, sizeof(AUDIO_SAMPLE));
			m_list.RemoveAt(s);
			m_size -= tag.size;
			return tag.size > 0;
		}
		return FALSE;
	}

	BOOL MAudioQueue::Pop(AUDIO_SAMPLE& tag, INT& count)
	{
		TinyAutoLock lock(m_lock);
		count = 0;
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			AUDIO_SAMPLE& sampleTag = m_list.GetAt(s);
			memcpy(&tag, &sampleTag, sizeof(AUDIO_SAMPLE));
			m_list.RemoveAt(s);
			count = m_list.GetSize();
			m_size -= tag.size;
			return tag.size > 0;
		}
		return FALSE;
	}

	BOOL MAudioQueue::IsEmpty()
	{
		return m_list.IsEmpty();
	}

	LONG MAudioQueue::GetSize()
	{
		return m_size;
	}

	DWORD MAudioQueue::GetCount()
	{
		TinyAutoLock lock(m_lock);
		return m_list.GetSize();
	}

	void MAudioQueue::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		ITERATOR pos = m_list.First();
		m_list.RemoveAll();
		m_alloc.FreeAll();
		m_size = 0;
	}
}

