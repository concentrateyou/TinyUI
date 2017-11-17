#pragma once
#include "MShowCommon.h"

namespace MShow
{
#define MAX_QUEUE_SIZE (512 * 1024)
#define MAX_AUDIO_QUEUE_SIZE (512 * 1024)   //PCM����
#define MAX_VIDEO_QUEUE_SIZE (16 * 1024 * 1024) //RGBA����
	/// <summary>
	/// ���ݶ���
	/// </summary>
	class MPacketQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MPacketQueue)
	public:
		MPacketQueue();
		~MPacketQueue();
		BOOL Push(SampleTag& tag);
		BOOL Pop(SampleTag& tag);
		BOOL Pop(SampleTag& tag,INT& count);
		BOOL IsEmpty();
		LONG GetSize();
		DWORD GetCount();
		void RemoveAll();
	private:
		LONG					m_size;
		TinyLock				m_lock;
		TinyLinkList<SampleTag>	m_list;
	};
	/// <summary>
	/// �̶�Ԫ�ش�С�Ļ���ض���
	/// </summary>
	class MPacketAllocQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MPacketAllocQueue)
	public:
		MPacketAllocQueue();
		~MPacketAllocQueue();
		void Initialize(UINT count, UINT size);
		UINT GetAllocSize();
		void* Alloc();
		void Free(void* p);
		BOOL Push(SampleTag& tag);
		BOOL Pop(SampleTag& tag);
		BOOL IsEmpty();
		LONG GetSize();
		DWORD GetCount();
		void RemoveAll();
	private:
		LONG					m_size;
		TinyLock				m_lock;
		TinyFixedAlloc			m_alloc;
		TinyLinkList<SampleTag>	m_list;
	};
}



