#pragma once
#include "../Common/TinyCommon.h"
#include "../Common/TinyString.h"
#include <string>
#include <memory>
#include <vector>
using namespace TinyUI;

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPAttribute
		{
		protected:
			struct KeyValue
			{
				KeyValue();
				KeyValue(const std::string& k, const std::string& v);
				KeyValue(KeyValue&& other);
				KeyValue& operator= (KeyValue&& other);
				std::string key;
				std::string value;
			};
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPAttribute)
		public:
			TinyHTTPAttribute();
			void Add(const string& key, const string& val);
			void Remove(const string& key);
		public:
			string	operator[](const string& key);
		public:
			std::vector<TinyHTTPAttribute::KeyValue>::const_iterator Lookup(const string& key) const;
			std::vector<TinyHTTPAttribute::KeyValue>::iterator Lookup(const string& key);
		protected:
			std::vector<KeyValue>	m_attributes;
		};
	}
}

