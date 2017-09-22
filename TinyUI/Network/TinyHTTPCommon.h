#pragma once
#include "../Common/TinyCommon.h"
#include "../Common/TinyString.h"
#include <string>
#include <memory>
#include <vector>
using namespace TinyUI;
using namespace std;

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
			BOOL IsEmpty() const;
			BOOL Include(const string& key);
			void Add(const string& key, const string& val);
			void Remove(const string& key);
			void RemoveAll();
			string ToString() const;
		public:
			string	operator[](const string& key);
			string	GetAttribute(const string& key);
		public:
			vector<TinyHTTPAttribute::KeyValue>::const_iterator Lookup(const string& key) const;
			vector<TinyHTTPAttribute::KeyValue>::iterator Lookup(const string& key);
		protected:
			vector<KeyValue>	m_attributes;
		};
	}
}

