#include "../stdafx.h"
#include "TinyStringPiece.h"

namespace TinyFramework
{
	namespace internal
	{
		inline void BuildLookupTable(const StringPiece& piece, bool* table)
		{
			const std::size_t length = piece.length();
			const char* const data = piece.data();
			for (std::size_t i = 0; i < length; ++i)
			{
				table[static_cast<unsigned char>(data[i])] = true;
			}
		}
		template<typename STR>
		void CopyToStringT(const BasicStringPiece<STR>& self, STR* target)
		{
			if (self.empty())
				target->clear();
			else
				target->assign(self.data(), self.size());
		}
		void CopyToString(const StringPiece& self, std::string* target)
		{
			CopyToStringT(self, target);
		}
		void CopyToString(const StringPiece16& self, std::wstring* target)
		{
			CopyToStringT(self, target);
		}
		template<typename STR>
		void AppendToStringT(const BasicStringPiece<STR>& self, STR* target)
		{
			if (!self.empty())
				target->append(self.data(), self.size());
		}
		void AppendToString(const StringPiece& self, std::string* target)
		{
			AppendToStringT(self, target);
		}
		void AppendToString(const StringPiece16& self, std::wstring* target)
		{
			AppendToStringT(self, target);
		}
		template<typename STR>
		std::size_t copyT(const BasicStringPiece<STR>& self, typename STR::value_type* buf, std::size_t n, std::size_t pos)
		{
			std::size_t ret = std::min<std::size_t>(self.size() - pos, n);
			memcpy(buf, self.data() + pos, ret * sizeof(typename STR::value_type));
			return ret;
		}
		std::size_t copy(const StringPiece& self, char* buf, std::size_t n, std::size_t pos)
		{
			return copyT(self, buf, n, pos);
		}
		std::size_t copy(const StringPiece16& self, WCHAR* buf, std::size_t n, std::size_t pos)
		{
			return copyT(self, buf, n, pos);
		}
		template<typename STR>
		std::size_t findT(const BasicStringPiece<STR>& self, const BasicStringPiece<STR>& s, std::size_t pos)
		{
			if (pos > self.size())
				return BasicStringPiece<STR>::npos;
			typename BasicStringPiece<STR>::const_iterator result = std::search(self.begin() + pos, self.end(), s.begin(), s.end());
			const size_t xpos = static_cast<size_t>(result - self.begin());
			return xpos + s.size() <= self.size() ? xpos : BasicStringPiece<STR>::npos;
		}
		std::size_t find(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			return findT(self, s, pos);
		}
		std::size_t find(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			return findT(self, s, pos);
		}
		template<typename STR>
		std::size_t findT(const BasicStringPiece<STR>& self, typename STR::value_type c, std::size_t pos)
		{
			if (pos >= self.size())
				return BasicStringPiece<STR>::npos;

			typename BasicStringPiece<STR>::const_iterator result =
				std::find(self.begin() + pos, self.end(), c);
			return result != self.end() ?
				static_cast<size_t>(result - self.begin()) : BasicStringPiece<STR>::npos;
		}
		std::size_t find(const StringPiece& self, char c, std::size_t pos)
		{
			return findT(self, c, pos);
		}
		std::size_t find(const StringPiece16& self, WCHAR c, std::size_t pos)
		{
			return findT(self, c, pos);
		}
		template<typename STR>
		std::size_t rfindT(const BasicStringPiece<STR>& self, const BasicStringPiece<STR>& s, std::size_t pos)
		{
			if (self.size() < s.size())
				return BasicStringPiece<STR>::npos;
			if (s.empty())
				return std::min<std::size_t>(self.size(), pos);
			typename BasicStringPiece<STR>::const_iterator last =
				self.begin() + std::min<std::size_t>(self.size() - s.size(), pos) + s.size();
			typename BasicStringPiece<STR>::const_iterator result =
				std::find_end(self.begin(), last, s.begin(), s.end());
			return result != last ?
				static_cast<size_t>(result - self.begin()) : BasicStringPiece<STR>::npos;
		}
		std::size_t rfind(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			return rfindT(self, s, pos);
		}
		std::size_t rfind(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			return rfindT(self, s, pos);
		}
		template<typename STR>
		std::size_t rfindT(const BasicStringPiece<STR>& self, typename STR::value_type c, std::size_t pos)
		{
			if (self.size() == 0)
				return BasicStringPiece<STR>::npos;

			for (std::size_t i = std::min<std::size_t>(pos, self.size() - 1); ; --i)
			{
				if (self.data()[i] == c)
					return i;
				if (i == 0)
					break;
			}
			return BasicStringPiece<STR>::npos;
		}
		std::size_t rfind(const StringPiece& self, char c, std::size_t pos)
		{
			return rfindT(self, c, pos);
		}
		std::size_t rfind(const StringPiece16& self, WCHAR c, std::size_t pos)
		{
			return rfindT(self, c, pos);
		}
		std::size_t find_first_of(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			if (self.size() == 0 || s.size() == 0)
				return StringPiece::npos;

			if (s.size() == 1)
				return find(self, s.data()[0], pos);

			bool lookup[UCHAR_MAX + 1] = { false };
			BuildLookupTable(s, lookup);
			for (size_t i = pos; i < self.size(); ++i)
			{
				if (lookup[static_cast<unsigned char>(self.data()[i])])
				{
					return i;
				}
			}
			return StringPiece::npos;
		}
		std::size_t find_first_of(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			StringPiece16::const_iterator found = std::find_first_of(self.begin() + pos, self.end(), s.begin(), s.end());
			if (found == self.end())
				return StringPiece16::npos;
			return found - self.begin();
		}
		std::size_t find_first_not_of(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			if (self.size() == 0)
				return StringPiece::npos;
			if (s.size() == 0)
				return 0;
			if (s.size() == 1)
				return find_first_not_of(self, s.data()[0], pos);

			bool lookup[UCHAR_MAX + 1] = { false };
			BuildLookupTable(s, lookup);
			for (size_t i = pos; i < self.size(); ++i)
			{
				if (!lookup[static_cast<unsigned char>(self.data()[i])])
				{
					return i;
				}
			}
			return StringPiece::npos;
		}
		std::size_t find_first_not_of(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			if (self.size() == 0)
				return StringPiece16::npos;

			for (size_t self_i = pos; self_i < self.size(); ++self_i)
			{
				bool found = false;
				for (size_t s_i = 0; s_i < s.size(); ++s_i)
				{
					if (self[self_i] == s[s_i])
					{
						found = true;
						break;
					}
				}
				if (!found)
					return self_i;
			}
			return StringPiece16::npos;
		}
		template<typename STR>
		std::size_t find_first_not_ofT(const BasicStringPiece<STR>& self, typename STR::value_type c, std::size_t pos)
		{
			if (self.size() == 0)
				return BasicStringPiece<STR>::npos;

			for (; pos < self.size(); ++pos)
			{
				if (self.data()[pos] != c)
				{
					return pos;
				}
			}
			return BasicStringPiece<STR>::npos;
		}
		std::size_t find_first_not_of(const StringPiece& self, char c, std::size_t pos)
		{
			return find_first_not_ofT(self, c, pos);
		}
		std::size_t find_first_not_of(const StringPiece16& self, WCHAR c, std::size_t pos)
		{
			return find_first_not_ofT(self, c, pos);
		}
		std::size_t find_last_of(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			if (self.size() == 0 || s.size() == 0)
				return StringPiece::npos;

			if (s.size() == 1)
				return rfind(self, s.data()[0], pos);

			bool lookup[UCHAR_MAX + 1] = { false };
			BuildLookupTable(s, lookup);
			for (size_t i = std::min<std::size_t>(pos, self.size() - 1); ; --i)
			{
				if (lookup[static_cast<unsigned char>(self.data()[i])])
					return i;
				if (i == 0)
					break;
			}
			return StringPiece::npos;
		}
		std::size_t find_last_of(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			if (self.size() == 0)
				return StringPiece16::npos;

			for (std::size_t self_i = std::min<std::size_t>(pos, self.size() - 1); ;--self_i)
			{
				for (std::size_t s_i = 0; s_i < s.size(); s_i++)
				{
					if (self.data()[self_i] == s[s_i])
						return self_i;
				}
				if (self_i == 0)
					break;
			}
			return StringPiece16::npos;
		}
		std::size_t find_last_not_of(const StringPiece& self, const StringPiece& s, std::size_t pos)
		{
			if (self.size() == 0)
				return StringPiece::npos;
			std::size_t i = std::min<std::size_t>(pos, self.size() - 1);
			if (s.size() == 0)
				return i;
			if (s.size() == 1)
				return find_last_not_of(self, s.data()[0], pos);
			bool lookup[UCHAR_MAX + 1] = { false };
			BuildLookupTable(s, lookup);
			for (; ; --i)
			{
				if (!lookup[static_cast<unsigned char>(self.data()[i])])
					return i;
				if (i == 0)
					break;
			}
			return StringPiece::npos;
		}
		std::size_t find_last_not_of(const StringPiece16& self, const StringPiece16& s, std::size_t pos)
		{
			if (self.size() == 0)
				return StringPiece::npos;

			for (size_t self_i = std::min<std::size_t>(pos, self.size() - 1); ; --self_i)
			{
				bool found = false;
				for (size_t s_i = 0; s_i < s.size(); s_i++)
				{
					if (self.data()[self_i] == s[s_i])
					{
						found = true;
						break;
					}
				}
				if (!found)
					return self_i;
				if (self_i == 0)
					break;
			}
			return StringPiece16::npos;
		};
		template<typename STR>
		std::size_t find_last_not_ofT(const BasicStringPiece<STR>& self, typename STR::value_type c, std::size_t pos)
		{
			if (self.size() == 0)
				return BasicStringPiece<STR>::npos;

			for (size_t i = std::min<std::size_t>(pos, self.size() - 1); ; --i)
			{
				if (self.data()[i] != c)
					return i;
				if (i == 0)
					break;
			}
			return BasicStringPiece<STR>::npos;
		};
		std::size_t find_last_not_of(const StringPiece& self, char c, std::size_t pos)
		{
			return find_last_not_ofT(self, c, pos);
		}
		std::size_t find_last_not_of(const StringPiece16& self, WCHAR c, std::size_t pos)
		{
			return find_last_not_ofT(self, c, pos);
		}
		template<typename STR>
		BasicStringPiece<STR> substrT(const BasicStringPiece<STR>& self, std::size_t pos, std::size_t n)
		{
			if (pos > self.size()) pos = self.size();
			if (n > self.size() - pos) n = self.size() - pos;
			return BasicStringPiece<STR>(self.data() + pos, n);
		}
		StringPiece substr(const StringPiece& self, std::size_t pos, std::size_t n)
		{
			return substrT(self, pos, n);
		}
		StringPiece16 substr(const StringPiece16& self, std::size_t pos, std::size_t n)
		{
			return substrT(self, pos, n);
		}
	}
}