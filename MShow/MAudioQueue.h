#pragma once
#include "MShowCommon.h"

namespace MShow
{
	typedef struct tagAUDIO_SAMPLE
	{
		BYTE*		bits;
		LONG		size;
		LONGLONG	timestamp;
	}AUDIO_SAMPLE;

	class MAudioQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioQueue)
	public:
		MAudioQueue();
		~MAudioQueue();
		void Initialize(UINT count, UINT size);
		UINT GetAllocSize();
		void* Alloc();
		void Free(void* p);
		BOOL Push(AUDIO_SAMPLE& tag);
		BOOL Pop(AUDIO_SAMPLE& tag);
		BOOL Pop(AUDIO_SAMPLE& tag, INT& count);
		BOOL IsEmpty();
		LONG GetSize();
		DWORD GetCount();
		void RemoveAll();
	private:
		LONG						m_size;
		TinyLock					m_lock;
		TinyFixedAlloc				m_alloc;
		TinyLinkList<AUDIO_SAMPLE>	m_list;
	};
}



