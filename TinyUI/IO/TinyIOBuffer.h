#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		class IOBuffer : public TinyReference<IOBuffer>
		{
		public:
			IOBuffer();
			explicit IOBuffer(INT size);
			CHAR* data() const;
		protected:
			friend class TinyReference<IOBuffer>;
			explicit IOBuffer(CHAR* data);
			virtual ~IOBuffer();
			CHAR*	m_data;
		};
		class DrainableIOBuffer : public IOBuffer
		{
		public:
			DrainableIOBuffer(IOBuffer* base, INT size);
			virtual ~DrainableIOBuffer();
			void	SetOffset(INT offset);
			INT		GetOffset() const;
			INT		GetRemaining() const;
			INT		size() const;
		private:
			INT									m_offset;
			INT									m_size;
			TinyScopedReferencePtr<IOBuffer>	m_base;
		};
		/// <summary>
		/// ÍøÂç»º³å
		/// </summary>
		class NetworkIOBuffer : public IOBuffer
		{
		public:
			NetworkIOBuffer();
			void SetCapacity(INT capacity);
			INT capacity() const;
			void SetOffset(INT offset);
			INT offset() const;
			void SetReceive(INT receive);
			INT  receive() const;
			INT RemainingCapacity();
			CHAR* StartOfBuffer();
		protected:
			virtual ~NetworkIOBuffer();
		protected:
			INT									m_receive;
			INT									m_capacity;
			INT									m_offset;
			TinyScopedPtr<CHAR, FreeDeleter>	m_io;
		};
		/// <summary>
		/// ¶¯Ì¬»º³åÇø
		/// </summary>
		class GrowableIOBuffer : public IOBuffer
		{
		public:
			GrowableIOBuffer();
			virtual ~GrowableIOBuffer();
			void SetCapacity(INT capacity);
			INT capacity() const;
			void SetOffset(INT offset);
			INT offset() const;
			INT RemainingCapacity();
			CHAR* StartOfBuffer();
		protected:
			INT									m_capacity;
			INT									m_offset;
			TinyScopedPtr<CHAR, FreeDeleter>	m_io;
		};
	};
}

