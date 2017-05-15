#pragma once
#include "Utility.h"

class PacketQueue
{
public:
	PacketQueue(TinyLock& lock);
	~PacketQueue();
	void Push(SampleTag& tag);
	SampleTag Pop();
	INT GetSize() const;
private:
	INT						m_size;
	TinyLock&				m_lock;
	TinyLinkList<SampleTag>	m_list;
};

