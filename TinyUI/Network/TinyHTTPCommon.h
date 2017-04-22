#pragma once
#include "../Common/TinyCommon.h"
#include <string>
#include <memory>
using namespace TinyUI;

namespace TinyUI
{
	namespace Network
	{
		class IOBuffer : public TinyReference<IOBuffer>
		{
		public:
			IOBuffer();
			explicit IOBuffer(INT size);
			CHAR*	data() const;
			INT		size() const;
		protected:
			friend class TinyReference<IOBuffer>;
			explicit IOBuffer(CHAR* data, INT size);
			virtual ~IOBuffer();
			CHAR*	m_data;
			INT		m_size;
		};

		class StringIOBuffer : public IOBuffer
		{
		public:
			explicit StringIOBuffer(const std::string& s);
			explicit StringIOBuffer(std::unique_ptr<std::string> s);
			INT size() const;
		private:
			virtual ~StringIOBuffer() OVERRIDE;
			std::string m_value;
		};

		class DrainableIOBuffer : public IOBuffer
		{
		public:
			DrainableIOBuffer(IOBuffer* base);
			INT Remaining() const;
			INT Consume() const;
			void SetConsume(INT bytes);
			void SetOffset(INT bytes);
		private:
			~DrainableIOBuffer() OVERRIDE;
			TinyScopedReferencePtr<IOBuffer> m_base;
			INT m_bytes;
		};
	}
}

