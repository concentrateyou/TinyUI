#pragma once
#include "MediaSDK.h"
#include <memory>
using namespace std;

namespace MediaSDK
{
	class ByteQueue
	{
		DISALLOW_COPY_AND_ASSIGN(ByteQueue)
	public:
		ByteQueue();
		~ByteQueue();
		void Reset();
		void Push(const CHAR* data, INT32 size);
		void Peek(const CHAR** data, INT32* size) const;
		void Pop(INT32 count);
	private:
		CHAR* front() const;
		unique_ptr<CHAR[]> m_buffer;
		size_t	m_size;
		size_t	m_offset;
		INT32	m_current;
	};
}


