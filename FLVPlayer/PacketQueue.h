#pragma once
#include "Common.h"

class PacketQueue
{
public:
	PacketQueue(TinyLock& lock);
	~PacketQueue();
	void Push(Decode::SampleTag& tag);
	Decode::SampleTag Pop();
	INT GetSize() const;
	BOOL IsEmpty() const;
	INT GetCount() const;
private:
	INT						m_size;
	TinyLock&				m_lock;
	TinyLinkList<Decode::SampleTag>	m_list;
};

