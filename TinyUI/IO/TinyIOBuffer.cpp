#include "../stdafx.h"
#include <process.h>
#include "TinyIOBuffer.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		//////////////////////////////////////////////////////////////////////////
		IOBuffer::IOBuffer()
			:m_data(NULL)
		{

		}
		IOBuffer::IOBuffer(INT size)
			: m_data(NULL)
		{
			m_data = new CHAR[size];
		}
		IOBuffer::IOBuffer(CHAR* data)
			: m_data(data)
		{

		}
		IOBuffer::~IOBuffer()
		{
			SAFE_DELETE_ARRAY(m_data);
		}
		CHAR* IOBuffer::data() const
		{
			return m_data;
		}
		//////////////////////////////////////////////////////////////////////////
		DrainableIOBuffer::DrainableIOBuffer(IOBuffer* base, INT size)
			:IOBuffer(base->data()),
			m_base(base),
			m_size(size),
			m_offset(0)
		{

		}
		void DrainableIOBuffer::SetOffset(INT offset)
		{
			m_offset = offset;
			m_data = m_base->data() + offset;
		}
		INT  DrainableIOBuffer::GetOffset() const
		{
			return m_offset;
		}
		INT  DrainableIOBuffer::GetRemaining() const
		{
			return m_size - m_offset;
		}
		INT DrainableIOBuffer::size() const
		{
			return m_size;
		}
		DrainableIOBuffer::~DrainableIOBuffer()
		{
			m_data = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		NetworkIOBuffer::NetworkIOBuffer()
			:m_capacity(0),
			m_offset(0),
			m_receive(0)
		{

		}
		void NetworkIOBuffer::SetCapacity(INT capacity)
		{
			m_io.Reset(static_cast<CHAR*>(realloc(m_io.Release(), capacity)));
			m_capacity = capacity;
			if (m_offset > capacity)
			{
				SetOffset(capacity);
			}
			else
			{
				SetOffset(m_offset);
				ZeroMemory(m_data, m_capacity - m_offset);
			}
		}
		INT NetworkIOBuffer::capacity() const
		{
			return m_capacity;
		}
		void NetworkIOBuffer::SetOffset(INT offset)
		{
			m_offset = offset;
			m_data = m_io.Ptr() + offset;
		}
		INT NetworkIOBuffer::offset() const
		{
			return m_offset;
		}
		void NetworkIOBuffer::SetReceive(INT receive)
		{
			m_receive = receive;
		}
		INT  NetworkIOBuffer::receive() const
		{
			return m_receive;
		}
		INT NetworkIOBuffer::RemainingCapacity()
		{
			return m_capacity - m_offset;
		}
		CHAR* NetworkIOBuffer::StartOfBuffer()
		{
			return m_io.Ptr();
		}
		NetworkIOBuffer::~NetworkIOBuffer()
		{
			m_data = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		GrowableIOBuffer::GrowableIOBuffer()
			:m_capacity(0),
			m_offset(0)
		{

		}
		GrowableIOBuffer::~GrowableIOBuffer()
		{
			m_data = NULL;
		}
		void GrowableIOBuffer::SetCapacity(INT capacity)
		{
			m_io.Reset(static_cast<CHAR*>(realloc(m_io.Release(), capacity)));
			m_capacity = capacity;
			if (m_offset > capacity)
			{
				SetOffset(capacity);
			}
			else
			{
				SetOffset(m_offset);
				ZeroMemory(m_data, m_capacity - m_offset);
			}
		}
		INT GrowableIOBuffer::capacity() const
		{
			return m_capacity;
		}
		void GrowableIOBuffer::SetOffset(INT offset)
		{
			m_offset = offset;
			m_data = m_io.Ptr() + offset;
		}
		INT GrowableIOBuffer::offset() const
		{
			return m_offset;
		}
		INT GrowableIOBuffer::RemainingCapacity()
		{
			return m_capacity - m_offset;
		}
		CHAR* GrowableIOBuffer::StartOfBuffer()
		{
			return m_io.Ptr();
		}
	}
}
