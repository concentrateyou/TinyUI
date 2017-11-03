#include "stdafx.h"
#include "MPacketQueue.h"

namespace MShow
{
	MPacketQueue::MPacketQueue()
		:m_size(0)
	{
	}

	MPacketQueue::~MPacketQueue()
	{
	}
	BOOL MPacketQueue::Push(Media::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.InsertLast(tag) != NULL)
		{
			m_size += tag.size;
			return TRUE;
		}
		return FALSE;
	}
	BOOL MPacketQueue::Pop(Media::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			Media::SampleTag& sampleTag = m_list.GetAt(s);
			memcpy(&tag, &sampleTag, sizeof(Media::SampleTag));
			m_list.RemoveAt(s);
			m_size -= tag.size;
			return (tag.size > 0);
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
			Media::SampleTag& tag = m_list.GetAt(pos);
			SAFE_DELETE_ARRAY(tag.bits);
			pos = m_list.Next(pos);
		}
		m_list.RemoveAll();
		m_size = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	MPacketAllocQueue::MPacketAllocQueue()
		:m_size(0)
	{

	}

	MPacketAllocQueue::~MPacketAllocQueue()
	{

	}

	UINT MPacketAllocQueue::GetAllocSize()
	{
		return m_alloc.GetAllocSize();
	}

	void MPacketAllocQueue::Initialize(UINT count, UINT size)
	{
		m_alloc.Initialize(count, size);
	}

	void* MPacketAllocQueue::Alloc()
	{
		return m_alloc.Alloc();
	}

	void MPacketAllocQueue::Free(void* p)
	{
		m_alloc.Free(p);
	}

	BOOL MPacketAllocQueue::Push(Media::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.InsertLast(tag) != NULL)
		{
			m_size += tag.size;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MPacketAllocQueue::Pop(Media::SampleTag& tag)
	{
		TinyAutoLock lock(m_lock);
		if (m_list.GetSize() > 0)
		{
			ITERATOR s = m_list.First();
			Media::SampleTag& sampleTag = m_list.GetAt(s);
			memcpy(&tag, &sampleTag, sizeof(Media::SampleTag));
			m_list.RemoveAt(s);
			m_size -= tag.size;
			return (tag.size > 0);
		}
		return FALSE;
	}

	BOOL MPacketAllocQueue::IsEmpty()
	{
		return m_list.IsEmpty();
	}

	LONG MPacketAllocQueue::GetSize()
	{
		return m_size;
	}

	DWORD MPacketAllocQueue::GetCount()
	{
		return m_list.GetSize();
	}

	void MPacketAllocQueue::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		ITERATOR pos = m_list.First();
		m_list.RemoveAll();
		m_alloc.FreeAll();
		m_size = 0;
	}
}
