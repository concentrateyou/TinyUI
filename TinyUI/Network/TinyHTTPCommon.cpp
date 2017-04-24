#include "../stdafx.h"
#include "TinyHTTPCommon.h"

namespace TinyUI
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
		std::vector<TinyHTTPAttribute::KeyValue>::const_iterator TinyHTTPAttribute::Lookup(const string& key) const
		{
			for (std::vector<TinyHTTPAttribute::KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (strncasecmp(key.c_str(), s->key.c_str(), key.size()) == 0)
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
			std::vector<TinyHTTPAttribute::KeyValue>::iterator s = Lookup(key);
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
			std::vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				m_attributes.erase(s);
			}
		}
		string TinyHTTPAttribute::operator[](const string& key)
		{
			std::vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				return s->value;
			}
			return string();
		}
		std::vector<TinyHTTPAttribute::KeyValue>::iterator TinyHTTPAttribute::Lookup(const string& key)
		{
			for (std::vector<TinyHTTPAttribute::KeyValue>::iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
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