#pragma once

namespace MShow
{
	/// <summary>
	/// 数据队列
	/// </summary>
	class MPacketQueue
	{
	public:
		MPacketQueue(TinyLock& lock);
		~MPacketQueue();
		void Push(Decode::SampleTag& tag);
		Decode::SampleTag Pop();
		BOOL IsEmpty() const;
		INT GetSize() const;
		INT GetCount() const;
	private:
		LONG					m_size;
		TinyLock&				m_lock;
		TinyLinkList<Decode::SampleTag>	m_tags;
	};
}



