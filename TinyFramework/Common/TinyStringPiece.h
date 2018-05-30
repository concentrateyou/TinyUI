#pragma once
#include "../Common/TinyCollection.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyFramework
{
	template <typename STRING_TYPE> class BasicStringPiece;
	typedef BasicStringPiece<std::string> StringPiece;
	typedef BasicStringPiece<std::wstring> StringPiece16;

	namespace internal
	{
		void CopyToString(const StringPiece& self, std::string* target);
		void CopyToString(const StringPiece16& self, std::wstring* target);
		void AppendToString(const StringPiece& self, std::string* target);
		void AppendToString(const StringPiece16& self, std::wstring* target);
		size_t copy(const StringPiece& self, char* buf, size_t n, size_t pos);
		size_t copy(const StringPiece16& self, WCHAR* buf, size_t n, size_t pos);
		size_t find(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t find(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t find(const StringPiece& self, char c, size_t pos);
		size_t find(const StringPiece16& self, WCHAR c, size_t pos);
		size_t rfind(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t rfind(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t rfind(const StringPiece& self, char c, size_t pos);
		size_t rfind(const StringPiece16& self, WCHAR c, size_t pos);
		size_t find_first_of(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t find_first_of(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t find_first_not_of(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t find_first_not_of(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t find_first_not_of(const StringPiece& self, char c, size_t pos);
		size_t find_first_not_of(const StringPiece16& self, WCHAR c, size_t pos);
		size_t find_last_of(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t find_last_of(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t find_last_of(const StringPiece& self, char c, size_t pos);
		size_t find_last_of(const StringPiece16& self, WCHAR c, size_t pos);
		size_t find_last_not_of(const StringPiece& self, const StringPiece& s, size_t pos);
		size_t find_last_not_of(const StringPiece16& self, const StringPiece16& s, size_t pos);
		size_t find_last_not_of(const StringPiece16& self, WCHAR c, size_t pos);
		size_t find_last_not_of(const StringPiece& self, char c, size_t pos);
		StringPiece substr(const StringPiece& self, size_t pos, size_t n);
		StringPiece16 substr(const StringPiece16& self, size_t pos, size_t n);
	}

	template <typename STRING_TYPE>
	class BasicStringPiece
	{
	public:
		typedef size_t size_type;
		typedef typename STRING_TYPE::value_type value_type;
		typedef const value_type* pointer;
		typedef const value_type& reference;
		typedef const value_type& const_reference;
		typedef ptrdiff_t difference_type;
		typedef const value_type* const_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		static const size_type npos;
	public:
		BasicStringPiece()
			: m_value(NULL), m_size(0)
		{
		}
		BasicStringPiece(const value_type* str)
			: m_value(str),
			m_size((str == NULL) ? 0 : STRING_TYPE::traits_type::length(str))
		{
		}
		BasicStringPiece(const STRING_TYPE& str)
			: m_value(str.data()), m_size(str.size())
		{
		}
		BasicStringPiece(const value_type* offset, size_type len)
			: m_value(offset), m_size(len)
		{
		}
		BasicStringPiece(const typename STRING_TYPE::const_iterator& begin, const typename STRING_TYPE::const_iterator& end)
		{
			m_size = static_cast<size_t>(std::distance(begin, end));
			m_value = m_size > 0 ? &*begin : nullptr;
		}
		const value_type* data() const
		{
			return m_value;
		}
		size_type size() const
		{
			return m_size;
		}
		size_type length() const
		{
			return m_size;
		}
		bool empty() const
		{
			return m_size == 0;
		}
		void clear()
		{
			m_value = NULL;
			m_size = 0;
		}
		void set(const value_type* data, size_type size)
		{
			m_value = data;
			m_size = size;
		}
		void set(const value_type* str)
		{
			m_value = str;
			m_size = str ? STRING_TYPE::traits_type::length(str) : 0;
		}
		value_type operator[](size_type i) const
		{
			return m_value[i];
		}
		value_type front() const
		{
			return m_value[0];
		}
		value_type back() const
		{
			return m_value[m_size - 1];
		}
		void remove_prefix(size_type n)
		{
			m_value += n;
			m_size -= n;
		}
		void remove_suffix(size_type n)
		{
			m_size -= n;
		}
		int compare(const BasicStringPiece<STRING_TYPE>& x) const
		{
			int r = wordmemcmp(m_value, x.ptr_, (m_size < x.length_ ? m_size : x.length_));
			if (r == 0)
			{
				if (m_size < x.length_)
					r = -1;
				else if (m_size > x.length_)
					r = +1;
			}
			return r;
		}
		STRING_TYPE as_string() const
		{
			return empty() ? STRING_TYPE() : STRING_TYPE(data(), size());
		}
		const_iterator begin() const
		{
			return m_value;
		}
		const_iterator end() const
		{
			return m_value + m_size;
		}
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(m_value + m_size);
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(m_value);
		}
		size_type max_size() const
		{
			return m_size;
		}
		size_type capacity() const
		{
			return m_size;
		}
		static int wordmemcmp(const value_type* p, const value_type* p2, size_type N)
		{
			return STRING_TYPE::traits_type::compare(p, p2, N);
		}
		void CopyToString(STRING_TYPE* target) const
		{
			internal::CopyToString(*this, target);
		}
		void AppendToString(STRING_TYPE* target) const
		{
			internal::AppendToString(*this, target);
		}
		size_type copy(value_type* buf, size_type n, size_type pos = 0) const
		{
			return internal::copy(*this, buf, n, pos);
		}
		bool starts_with(const BasicStringPiece& x) const
		{
			return ((this->m_size >= x.m_size) && (wordmemcmp(this->m_value, x.m_value, x.m_size) == 0));
		}
		bool ends_with(const BasicStringPiece& x) const
		{
			return ((this->m_size >= x.m_size) && (wordmemcmp(this->m_value + (this->m_size - x.m_size), x.m_value, x.m_size) == 0));
		}
		size_type find(const BasicStringPiece<STRING_TYPE>& s, size_type pos = 0) const
		{
			return internal::find(*this, s, pos);
		}
		size_type find(value_type c, size_type pos = 0) const
		{
			return internal::find(*this, c, pos);
		}
		size_type rfind(const BasicStringPiece& s, size_type pos = BasicStringPiece::npos) const
		{
			return internal::rfind(*this, s, pos);
		}
		size_type rfind(value_type c, size_type pos = BasicStringPiece::npos) const
		{
			return internal::rfind(*this, c, pos);
		}
		size_type find_first_of(const BasicStringPiece& s, size_type pos = 0) const
		{
			return internal::find_first_of(*this, s, pos);
		}
		size_type find_first_of(value_type c, size_type pos = 0) const
		{
			return find(c, pos);
		}
		size_type find_first_not_of(const BasicStringPiece& s, size_type pos = 0) const
		{
			return internal::find_first_not_of(*this, s, pos);
		}
		size_type find_first_not_of(value_type c, size_type pos = 0) const
		{
			return internal::find_first_not_of(*this, c, pos);
		}
		size_type find_last_of(const BasicStringPiece& s, size_type pos = BasicStringPiece::npos) const
		{
			return internal::find_last_of(*this, s, pos);
		}
		size_type find_last_of(value_type c, size_type pos = BasicStringPiece::npos) const
		{
			return rfind(c, pos);
		}
		size_type find_last_not_of(const BasicStringPiece& s, size_type pos = BasicStringPiece::npos) const
		{
			return internal::find_last_not_of(*this, s, pos);
		}
		size_type find_last_not_of(value_type c, size_type pos = BasicStringPiece::npos) const
		{
			return internal::find_last_not_of(*this, c, pos);
		}
		BasicStringPiece substr(size_type pos, size_type n = BasicStringPiece::npos) const
		{
			return internal::substr(*this, pos, n);
		}
	protected:
		const value_type*	m_value;
		size_type			m_size;
	};

	template <typename STRING_TYPE>
	const typename BasicStringPiece<STRING_TYPE>::size_type BasicStringPiece<STRING_TYPE>::npos = typename BasicStringPiece<STRING_TYPE>::size_type(-1);

	inline bool operator==(const StringPiece& x, const StringPiece& y)
	{
		if (x.size() != y.size())
			return false;
		return StringPiece::wordmemcmp(x.data(), y.data(), x.size()) == 0;
	}

	inline bool operator!=(const StringPiece& x, const StringPiece& y)
	{
		return !(x == y);
	}

	inline bool operator<(const StringPiece& x, const StringPiece& y)
	{
		const int r = StringPiece::wordmemcmp(x.data(), y.data(), (x.size() < y.size() ? x.size() : y.size()));
		return ((r < 0) || ((r == 0) && (x.size() < y.size())));
	}

	inline bool operator>(const StringPiece& x, const StringPiece& y)
	{
		return y < x;
	}

	inline bool operator<=(const StringPiece& x, const StringPiece& y)
	{
		return !(x > y);
	}

	inline bool operator>=(const StringPiece& x, const StringPiece& y)
	{
		return !(x < y);
	}

	inline bool operator==(const StringPiece16& x, const StringPiece16& y)
	{
		if (x.size() != y.size())
			return false;
		return StringPiece16::wordmemcmp(x.data(), y.data(), x.size()) == 0;
	}

	inline bool operator!=(const StringPiece16& x, const StringPiece16& y)
	{
		return !(x == y);
	}

	inline bool operator<(const StringPiece16& x, const StringPiece16& y)
	{
		const int r = StringPiece16::wordmemcmp(x.data(), y.data(), (x.size() < y.size() ? x.size() : y.size()));
		return ((r < 0) || ((r == 0) && (x.size() < y.size())));
	}

	inline bool operator>(const StringPiece16& x, const StringPiece16& y)
	{
		return y < x;
	}

	inline bool operator<=(const StringPiece16& x, const StringPiece16& y)
	{
		return !(x > y);
	}

	inline bool operator>=(const StringPiece16& x, const StringPiece16& y)
	{
		return !(x < y);
	}

	inline std::ostream& operator<<(std::ostream& o, const StringPiece& piece)
	{
		o.write(piece.data(), static_cast<std::streamsize>(piece.size()));
		return o;
	}
}

