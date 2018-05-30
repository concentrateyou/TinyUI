#include "../stdafx.h"
#include "TinyHTTPCommon.h"

namespace TinyFramework
{
	namespace Network
	{
		TinyHTTPAttribute::KeyValue::KeyValue()
		{

		}
		TinyHTTPAttribute::KeyValue::KeyValue(const std::string& k, const std::string& v)
			: key(std::move(k)),
			value(std::move(v))
		{

		}
		TinyHTTPAttribute::KeyValue::KeyValue(TinyHTTPAttribute::KeyValue&& other)
			: key(std::move(other.key)),
			value(std::move(other.value))
		{

		}
		TinyHTTPAttribute::KeyValue& TinyHTTPAttribute::KeyValue::operator = (TinyHTTPAttribute::KeyValue&& other)
		{
			if (this != &other)
			{
				key = std::move(other.key);
				value = std::move(other.value);
				other.key.clear();
				other.value.clear();
			}
			return *this;
		}
		vector<TinyHTTPAttribute::KeyValue>::const_iterator TinyHTTPAttribute::Lookup(const string& key) const
		{
			for (vector<TinyHTTPAttribute::KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				string key1 = ToLowerASCII(key);
				string key2 = ToLowerASCII(s->key);
				if (strncasecmp(key1.c_str(), key2.c_str(), key1.size()) == 0)
				{
					return s;
				}
			}
			return m_attributes.end();
		}
		TinyHTTPAttribute::TinyHTTPAttribute()
		{

		}
		void TinyHTTPAttribute::Add(const string& key, const string& value)
		{
			vector<TinyHTTPAttribute::KeyValue>::iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				s->value = std::move(value);
			}
			else
			{
				m_attributes.push_back(KeyValue(key, value));
			}
		}
		void TinyHTTPAttribute::Remove(const string& key)
		{
			vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				m_attributes.erase(s);
			}
		}
		BOOL TinyHTTPAttribute::IsEmpty() const
		{
			return m_attributes.empty();
		}
		BOOL TinyHTTPAttribute::Contains(const string& key)
		{
			vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				return TRUE;
			}
			return FALSE;
		}
		void TinyHTTPAttribute::RemoveAll()
		{
			m_attributes.clear();
		}
		string TinyHTTPAttribute::ToString() const
		{
			string output;
			for (vector<KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (!s->value.empty())
				{
					StringAppendF(&output, "%s: %s\r\n", s->key.c_str(), s->value.c_str());
				}
				else
				{
					StringAppendF(&output, "%s:\r\n", s->key.c_str());
				}
			}
			output.append("\r\n");
			return output;
		}
		string TinyHTTPAttribute::operator[](const string& key)
		{
			vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				return s->value;
			}
			return string();
		}
		string	TinyHTTPAttribute::GetAttribute(const string& key)
		{
			vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				return s->value;
			}
			return string();
		}
		vector<TinyHTTPAttribute::KeyValue>::iterator TinyHTTPAttribute::Lookup(const string& key)
		{
			for (vector<TinyHTTPAttribute::KeyValue>::iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (strncasecmp(key.c_str(), s->key.c_str(), key.size()) == 0)
				{
					return s;
				}
			}
			return m_attributes.end();
		}
	}
}