#include "../stdafx.h"
#include <stdarg.h>
#include "TinyString.h"

namespace TinyFramework
{
	INT strcasecmp(const char* s1, const char* s2)
	{
		return _stricmp(s1, s2);
	}
	INT strncasecmp(const char* s1, const char* s2, size_t count)
	{
		return _strnicmp(s1, s2, count);
	}
	INT strncmp16(const wchar_t* s1, const wchar_t* s2, size_t count)
	{
		return ::wcsncmp(s1, s2, count);
	}
	INT vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
	{
		INT length = vsnprintf_s(buffer, size, size - 1, format, arguments);
		if (length < 0)
		{
			return _vscprintf(format, arguments);
		}
		return length;
	}
	INT vswprintf(wchar_t* buffer, size_t size, const wchar_t* format, va_list arguments)
	{
		int length = _vsnwprintf_s(buffer, size, size - 1, format, arguments);
		if (length < 0)
		{
			return _vscwprintf(format, arguments);
		}
		return length;
	}
	string _cdecl StringPrintf(const CHAR* s, ...)
	{
		va_list args;
		va_start(args, s);
		INT size;
		CHAR szBuffer[1024];
		size = vsnprintf(szBuffer, 1024, s, args);
		ASSERT(size >= 0);
		va_end(args);
		return string(szBuffer, 0, size);
	}
	std::string WStringToString(const std::wstring& str, const DWORD dwType)
	{
		INT size = WideCharToMultiByte(dwType, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
		CHAR* data = new CHAR[size];
		size = WideCharToMultiByte(dwType, 0, str.c_str(), -1, data, size, NULL, NULL);
		std::string s = data;
		delete[] data;
		return s;
	}
	std::wstring StringToWString(const std::string& str, const DWORD dwType)
	{
		INT size = MultiByteToWideChar(dwType, 0, str.c_str(), -1, NULL, 0);
		WCHAR* data = new WCHAR[size];
		size = MultiByteToWideChar(dwType, 0, str.c_str(), -1, data, size);
		std::wstring s = data;
		delete[] data;
		return s;
	}
	std::string WStringToUTF8(const std::wstring& str)
	{
		return WStringToString(str, CP_UTF8);
	}
	std::string ASCIIToUTF8(const std::string& str)
	{
		std::wstring temp = StringToWString(str, CP_ACP);
		return WStringToString(temp, CP_UTF8);
	}
	std::string UTF8ToASCII(const std::string& str)
	{
		std::wstring temp = StringToWString(str, CP_UTF8);
		return WStringToString(temp, CP_ACP);
	}
	std::wstring UTF8ToUTF16(const std::string& s)
	{
		return StringToWString(UTF8ToASCII(s));
	}
	std::string UTF16ToUTF8(const std::wstring& wide)
	{
		return ASCIIToUTF8(WStringToString(wide));
	}
	//////////////////////////////////////////////////////////////////////////
	static void StringAppendVT(string* dst, const CHAR* format, va_list ap)
	{
		CHAR buffer[1024];
		va_list ap_copy;
		va_copy(ap_copy, ap);
		INT result = vsnprintf(buffer, arraysize(buffer), format, ap_copy);
		va_end(ap_copy);
		if (result >= 0 && result < static_cast<int>(arraysize(buffer)))
		{
			dst->append(buffer, result);
			return;
		}
		INT size = arraysize(buffer);
		while (true)
		{
			if (result < 0)
			{
				return;
			}
			else
			{
				size = result + 1;
			}

			if (size > 32 * 1024 * 1024)
			{
				return;
			}
			std::vector<CHAR> mem_buf(size);
			va_copy(ap_copy, ap);
			result = vsnprintf(&mem_buf[0], size, format, ap_copy);
			va_end(ap_copy);
			if ((result >= 0) && (result < size))
			{
				dst->append(&mem_buf[0], result);
				return;
			}
		}
	}
	void StringAppendF(string* dst, const CHAR* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		StringAppendVT(dst, format, ap);
		va_end(ap);
	}
	//////////////////////////////////////////////////////////////////////////
	TinyString::TinyString()
		:_Myres(0),
		_Mysize(0),
		_Mystr(NULL)
	{
		_Mystr = new CHAR[_Mysize + 1];
		_Mystr[_Mysize] = '\0';
	}
	TinyString::TinyString(size_t size)
		:_Myres(size),
		_Mysize(size),
		_Mystr(NULL)
	{
		_Mystr = new CHAR[_Mysize + 1];
		_Mystr[_Mysize] = '\0';
	}
	TinyString::TinyString(const CHAR* s)
		: _Myres(0),
		_Mysize(0),
		_Mystr(NULL)
	{
		if (s != NULL)
		{
			Assign(s, strlen(s));
		}
		else
		{
			_Mystr = new CHAR[_Mysize + 1];
			_Mystr[_Mysize] = '\0';
		}
	}
	TinyString::TinyString(const string& s)
		:_Myres(0),
		_Mysize(0),
		_Mystr(NULL)
	{
		if (!s.empty())
		{
			Assign(s);
		}
		else
		{
			_Mystr = new CHAR[_Mysize + 1];
			_Mystr[_Mysize] = '\0';
		}
	}
	TinyString::TinyString(const TinyString& s, size_t pos, size_t size)
		:_Myres(0),
		_Mysize(0),
		_Mystr(NULL)
	{
		Assign(s, pos, size);
	}
	TinyString::TinyString(const TinyString& s)
		:_Myres(0),
		_Mysize(0),
		_Mystr(NULL)
	{
		if (s._Mysize == 0)
		{
			_Mystr = new CHAR[_Mysize + 1];
			_Mystr[_Mysize] = '\0';
		}
		else
		{
			Assign(s, 0, s._Mysize);
		}
	}
	TinyString::TinyString(TinyString&& s)
	{
		this->_Mystr = s._Mystr;
		this->_Mysize = s._Mysize;
		this->_Myres = s._Myres;
		s._Myres = 0;
		s._Mysize = 0;
		s._Mystr = NULL;
	}
	TinyString::~TinyString()
	{
		SAFE_DELETE_ARRAY(this->_Mystr);
		this->_Myres = this->_Mysize = 0;
	}
	TinyString& TinyString::Append(const CHAR* s)
	{
		ASSERT(s);
		return Append(s, strlen(s));
	}
	TinyString& TinyString::Append(const CHAR* s, size_t _Newsize)
	{
		if (_Newsize > 0)
		{
			size_t Newsize = this->_Mysize + _Newsize;
			//»›¡ø≤ª◊„¿©≥‰
			if (this->_Myres < Newsize)
			{
				size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
				size_t _Newres = Newsize | this->_ALLOC_MASK;
				size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
				if (_Maxsize < _Newres)
					_Newres = Newsize;
				else if (_Newres < _size && _size <= _Maxsize)
					_Newres = _size;
				ASSERT(_Newres >= _Newsize);
				CHAR *_Ptr = NULL;
				_Ptr = new CHAR[_Newres + 1];
				memset(_Ptr, 0, _Newres + 1);
				if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
				{
					memcpy(_Ptr, this->_Mystr, this->_Mysize);
				}
				this->_Myres = _Newres;
				if (s)
				{
					memcpy(_Ptr + this->_Mysize, s, _Newsize);//∏Ωº”ƒ⁄¥Ê
				}
				this->_Mysize = Newsize;
				_Ptr[this->_Mysize] = '\0';
				SAFE_DELETE_ARRAY(this->_Mystr);
				this->_Mystr = _Ptr;
			}
			else//øΩ±¥ƒ⁄¥Ê
			{
				if (s != NULL)
				{
					memcpy(this->_Mystr + this->_Mysize, s, _Newsize);
				}
				this->_Mysize = Newsize;
				this->_Mystr[this->_Mysize] = '\0';
			}
		}
		return *this;
	}
	TinyString& TinyString::Append(const TinyString& str, size_t pos, size_t size)
	{
		size_t _Num = str._Mysize - pos;
		if (_Num < size)
			size = _Num;
		if (0 < size)
		{
			size_t Newsize = this->_Mysize + size;
			//»›¡ø≤ª◊„¿©≥‰
			if (this->_Myres < Newsize)
			{
				size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
				size_t _Newres = Newsize | this->_ALLOC_MASK;
				size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
				if (_Maxsize < _Newres)
					_Newres = Newsize;
				else if (_Newres < _size && _size <= _Maxsize)
					_Newres = _size;
				ASSERT(_Newres >= size);
				CHAR *_Ptr = NULL;
				_Ptr = new CHAR[_Newres + 1];
				if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
				{
					memcpy(_Ptr, this->_Mystr, this->_Mysize);
				}
				this->_Myres = _Newres;
				memcpy(_Ptr + this->_Mysize, str.STR() + pos, size);//∏Ωº”ƒ⁄¥Ê
				this->_Mysize = Newsize;
				_Ptr[this->_Mysize] = '\0';
				SAFE_DELETE_ARRAY(this->_Mystr);
				this->_Mystr = _Ptr;
			}
			else//øΩ±¥ƒ⁄¥Ê
			{
				memcpy(this->_Mystr + this->_Mysize, str.STR() + pos, size);
				this->_Mysize = Newsize;
				this->_Mystr[this->_Mysize] = '\0';
			}
		}
		return (*this);
	}
	TinyString& TinyString::Append(const TinyString& str)
	{
		return Append(str, 0, str._Mysize);
	}
	TinyString& TinyString::operator += (const TinyString& str)
	{
		return Append(str, 0, str._Mysize);
	}
	TinyString& TinyString::operator += (const CHAR* s)
	{
		return Append(s);
	}
	TinyString& TinyString::operator += (CHAR s)
	{
		return Append(&s);
	}
	TinyString& TinyString::operator = (TinyString&& str)
	{
		if (this != &str)
		{
			SAFE_DELETE_ARRAY(this->_Mystr);
			this->_Mystr = str._Mystr;
			this->_Mysize = str._Mysize;
			this->_Myres = str._Myres;
			str._Mystr = NULL;
			str._Mysize = 0;
			str._Myres = 0;
		}
		return *this;
	}
	TinyString& TinyString::operator = (const TinyString& str)
	{
		return Assign(str, 0, str._Mysize);
	}
	TinyString& TinyString::operator = (const CHAR* s)
	{
		return Assign(s);
	}
	TinyString& TinyString::operator = (CHAR s)
	{
		return Assign(&s);
	}
	BOOL TinyString::operator==(const TinyString& str) const
	{
		return this->Compare(str) == 0;
	}
	BOOL TinyString::operator == (const TinyString& str)
	{
		return this->Compare(str) == 0;
	}

	BOOL TinyString::operator == (const CHAR* s)
	{
		return strcmp(this->_Mystr, s) == 0;
	}
	BOOL TinyString::operator==(const CHAR* s) const
	{
		return strcmp(this->_Mystr, s) == 0;
	}
	BOOL TinyString::operator == (TinyString& str)
	{
		return this->Compare(str) == 0;
	}
	BOOL TinyString::operator == (const string& str)
	{
		return this->Compare(str) == 0;
	}
	BOOL TinyString::operator==(const string& str) const
	{
		return this->Compare(str) == 0;
	}
	BOOL TinyString::operator == (string& str)
	{
		return this->Compare(str) == 0;
	}
	BOOL TinyString::operator == (CHAR* s)
	{
		return strcmp(this->_Mystr, s) == 0;
	}

	BOOL TinyString::operator != (const TinyString& str)
	{
		return this->Compare(str) != 0;
	}

	BOOL TinyString::operator != (const CHAR* s)
	{
		return strcmp(this->_Mystr, s) != 0;
	}

	BOOL TinyString::operator != (TinyString& str)
	{
		return this->Compare(str) != 0;
	}
	BOOL TinyString::operator != (const string& str)
	{
		return this->Compare(str) != 0;
	}
	BOOL TinyString::operator != (string& str)
	{
		return this->Compare(str) != 0;
	}
	BOOL TinyString::operator != (CHAR* s)
	{
		return strcmp(this->_Mystr, s) != 0;
	}

	TinyString& TinyString::Assign(const CHAR* s)
	{
		ASSERT(s);
		return Assign(s, strlen(s));
	}
	TinyString& TinyString::Assign(const string& s)
	{
		return Assign(s.c_str(), s.size());
	}
	TinyString& TinyString::Assign(const CHAR* s, size_t _Newsize)
	{
		//»›¡ø≤ª◊„¿©≥‰
		if (this->_Myres < _Newsize)
		{
			size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
			size_t _Newres = _Newsize | this->_ALLOC_MASK;
			size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
			if (_Maxsize < _Newres)
				_Newres = _Newsize;
			else if (_Newres < _size && _size <= _Maxsize)
				_Newres = _size;
			ASSERT(_Newres >= _Newsize);
			CHAR *_Ptr = NULL;
			_Ptr = new CHAR[_Newres + 1];
			memcpy(_Ptr, s, _Newsize);
			this->_Myres = _Newres;
			this->_Mysize = _Newsize;
			_Ptr[this->_Mysize] = '\0';
			SAFE_DELETE_ARRAY(this->_Mystr);
			this->_Mystr = _Ptr;
		}
		else//øΩ±¥ƒ⁄¥Ê
		{
			if (this->_Mystr != NULL)
			{
				if (_Newsize > 0)
				{
					memcpy(this->_Mystr, s, _Newsize);
					this->_Mysize = _Newsize;
					this->_Mystr[this->_Mysize] = '\0';
				}
			}
			else
			{
				this->_Mystr = new CHAR[this->_Mysize + 1];
				this->_Mystr[this->_Mysize] = '\0';
			}
		}
		return *this;
	}
	TinyString& TinyString::Assign(const TinyString& str, size_t pos, size_t size)
	{
		if (str._Mysize <= pos)
		{
			this->_Mysize = 0;
			this->_Myres = 0;
			this->_Mystr = new CHAR[_Mysize + 1];
			this->_Mystr[this->_Mysize] = '\0';
		}
		else
		{
			size_t _Num = str._Mysize - pos;
			if (size < _Num)
				_Num = size;
			if (this == &str)//substring
			{
				Erase(pos + _Num);
				Erase(0, pos);
			}
			else
			{
				//»›¡ø≤ª◊„¿©≥‰
				if (this->_Myres < _Num)
				{
					size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
					size_t _Newres = _Num | this->_ALLOC_MASK;
					size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
					if (_Maxsize < _Newres)
						_Newres = _Num;
					else if (_Newres < _size && _size <= _Maxsize)
						_Newres = _size;
					ASSERT(_Newres >= _Num);
					CHAR *_Ptr = NULL;
					_Ptr = new CHAR[_Newres + 1];
					if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
					{
						memcpy(_Ptr, this->_Mystr, this->_Mysize);
					}
					this->_Myres = _Newres;
					memcpy(_Ptr, str.STR() + pos, _Num);//∏Ωº”ƒ⁄¥Ê
					this->_Mysize = _Num;
					_Ptr[this->_Mysize] = '\0';
					SAFE_DELETE_ARRAY(this->_Mystr);
					this->_Mystr = _Ptr;
				}
				else//øΩ±¥ƒ⁄¥Ê
				{
					memcpy(this->_Mystr, str.STR() + pos, _Num);//∏Ωº”ƒ⁄¥Ê
					this->_Mysize = _Num;
					this->_Mystr[this->_Mysize] = '\0';
				}
			}
		}
		return *this;
	}
	TinyString& TinyString::Erase(size_t pos)
	{
		ASSERT(this->_Mysize >= pos);
		this->_Mysize = pos;
		this->_Mystr[this->_Mysize] = '\0';
		return (*this);
	}
	TinyString& TinyString::Erase(size_t pos, size_t size)
	{
		ASSERT(this->_Mysize >= pos);
		if (this->_Mysize - pos <= size)
		{
			this->_Mysize = pos;
			this->_Mystr[this->_Mysize] = '\0';
		}
		else if (0 < size)
		{
			CHAR* _Ptr = this->_Mystr + pos;
			size_t _Newsize = this->_Mysize - size;
			memmove(_Ptr, _Ptr + size, _Newsize - pos);
			this->_Mysize = _Newsize;
			this->_Mystr[this->_Mysize] = '\0';
		}
		return (*this);
	}
	TinyString& TinyString::Insert(size_t pos, const CHAR* s)
	{
		ASSERT(s);
		return Insert(pos, s, strlen(s));
	}
	TinyString& TinyString::Insert(size_t pos, const CHAR* s, size_t size)
	{
		ASSERT(this->_Mysize >= pos);
		if (0 < size)
		{
			size_t _Num = this->_Mysize + size;
			//»›¡ø≤ª◊„¿©≥‰
			if (this->_Myres < _Num)
			{
				size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
				size_t _Newres = _Num | this->_ALLOC_MASK;
				size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
				if (_Maxsize < _Newres)
					_Newres = _Num;
				else if (_Newres < _size && _size <= _Maxsize)
					_Newres = _size;
				ASSERT(_Newres >= size);
				CHAR *_Ptr = NULL;
				_Ptr = new CHAR[_Newres + 1];
				if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
				{
					memcpy(_Ptr, this->_Mystr, this->_Mysize);
				}
				this->_Myres = _Newres;
				memmove(_Ptr + pos + size, _Ptr + pos, this->_Mysize - pos);
				memcpy(_Ptr + pos, s, size);
				this->_Mysize = _Num;
				_Ptr[this->_Mysize] = '\0';
				SAFE_DELETE_ARRAY(this->_Mystr);
				this->_Mystr = _Ptr;
			}
			else//øΩ±¥ƒ⁄¥Ê
			{
				memmove(this->_Mystr + pos + size, this->_Mystr + pos, this->_Mysize - pos);
				memcpy(this->_Mystr + pos, s, size);
				this->_Mysize = _Num;
				this->_Mystr[this->_Mysize] = '\0';
			}
		}
		return (*this);
	}
	TinyString& TinyString::Insert(size_t pos, const TinyString& str)
	{
		return Insert(pos, str, 0, str._Mysize);
	}
	TinyString& TinyString::Insert(size_t pos, const TinyString& str, size_t subpos, size_t subsize)
	{
		ASSERT(this->_Mysize >= pos && str._Mysize >= subpos);
		size_t _Num = str._Mysize - subpos;
		if (_Num < subsize)
			subsize = _Num;
		if (0 < subsize)
		{
			_Num = this->_Mysize + subsize;

			//»›¡ø≤ª◊„¿©≥‰
			if (this->_Myres < _Num)
			{
				size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
				size_t _Newres = _Num | this->_ALLOC_MASK;
				size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
				if (_Maxsize < _Newres)
					_Newres = _Num;
				else if (_Newres < _size && _size <= _Maxsize)
					_Newres = _size;
				ASSERT(_Newres >= subsize);
				CHAR *_Ptr = NULL;
				_Ptr = new CHAR[_Newres + 1];
				if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
				{
					memcpy(_Ptr, this->_Mystr, this->_Mysize);
				}
				this->_Myres = _Newres;
				memmove(_Ptr + pos + subsize, _Ptr + pos, this->_Mysize - pos);
				if (this == &str)
				{
					memmove(_Ptr + pos, _Ptr + (pos < subpos ? subpos + subsize : subpos), subsize);
				}
				else
				{
					memcpy(_Ptr + pos, str.STR() + subpos, subsize);
				}

				this->_Mysize = _Num;
				_Ptr[this->_Mysize] = '\0';
				SAFE_DELETE_ARRAY(this->_Mystr);
				this->_Mystr = _Ptr;
			}
			else//øΩ±¥ƒ⁄¥Ê
			{
				memmove(this->_Mystr + pos + subsize,
					this->_Mystr + pos,
					this->_Mysize - pos);
				if (this == &str)
				{
					memmove(this->_Mystr + pos,
						this->_Mystr + (pos < subpos ? subpos + subsize : subpos),
						subsize);
				}
				else
				{
					memcpy(this->_Mystr + pos, str.STR() + subpos, subsize);
				}
				this->_Mysize = _Num;
				this->_Mystr[this->_Mysize] = '\0';
			}
		}
		return (*this);
	}
	size_t	TinyString::GetSize() const
	{
		return _Mysize;
	}
	CHAR* TinyString::STR() const
	{
		return _Mystr;
	}
	const CHAR*	TinyString::CSTR() const
	{
		return _Mystr;
	}
	wstring	TinyString::ToWString() const
	{
		if (!_Mystr)
			return wstring();
		string szText(_Mystr);
		return StringToWString(szText);
	}
	string	TinyString::ToUTF8() const
	{
		if (!_Mystr)
			return string();
		string szText(_Mystr);
		return ASCIIToUTF8(szText);
	}
	void TinyString::Resize(size_t _Newsize)
	{
		if (_Newsize <= this->_Mysize)
		{
			Erase(_Newsize, this->_Mysize - _Newsize);
		}
		else
		{
			Append(NULL, _Newsize - this->_Mysize);
		}
	}
	TinyString _cdecl TinyString::Format(const CHAR* s, ...)
	{
		va_list args;
		va_start(args, s);
		INT size;
		CHAR szBuffer[512];
		size = _vsntprintf_s(szBuffer, 512, s, args);
		ASSERT(size >= 0);
		va_end(args);
		return TinyString(szBuffer, 0, size);
	}
	INT	TinyString::IndexOf(const CHAR* s, size_t pos) const
	{
		return IndexOf(s, pos, strlen(s));
	}
	INT	TinyString::IndexOf(const TinyString& str, size_t pos) const
	{
		return IndexOf(str.STR(), pos, str._Mysize);
	}
	INT	TinyString::IndexOf(const CHAR* s, size_t pos, size_t size) const
	{
		ASSERT(s);
		if (size == 0 && pos <= this->_Mysize)
			return pos;
		size_t _Nm = 0;
		if (pos < this->_Mysize && size <= (_Nm = this->_Mysize - pos))
		{
			const CHAR *_Uptr = NULL;
			const CHAR *_Vptr = NULL;

			for (_Nm -= size - 1, _Vptr = this->_Mystr + pos;
				(_Uptr = (CHAR*)memchr(_Vptr, *s, _Nm)) != 0;
				_Nm -= _Uptr - _Vptr + 1, _Vptr = _Uptr + 1)
			{
				if (memcmp(_Uptr, s, size) == 0)
				{
					return (_Uptr - this->_Mystr);
				}
			}
		}
		return -1;
	}
	INT	TinyString::IndexOf(CHAR s, size_t pos) const
	{
		return IndexOf(&s, pos, 1);
	}
	INT	TinyString::LastIndexOf(const CHAR* s, size_t pos) const
	{
		return LastIndexOf(s, pos, strlen(s));
	}
	INT	TinyString::LastIndexOf(const TinyString& str, size_t pos) const
	{
		return LastIndexOf(str.STR(), pos, str._Mysize);
	}
	INT	TinyString::LastIndexOf(const CHAR* s, size_t pos, size_t size) const
	{
		ASSERT(s);
		if (size == 0)
			return (pos < this->_Mysize ? pos : this->_Mysize);
		if (size <= this->_Mysize)
		{
			const CHAR *_Uptr = this->_Mystr + (pos < this->_Mysize - size ? pos : this->_Mysize - size);
			for (;; --_Uptr)
			{
				if ((*_Uptr == *s) && memcmp(_Uptr, s, size) == 0)
					return (_Uptr - this->_Mystr);
				else if (_Uptr == this->_Mystr)
					break;
			}
		}
		return -1;
	}
	INT	TinyString::LastIndexOf(CHAR s, size_t pos) const
	{
		return LastIndexOf(&s, pos, 1);
	}
	INT TinyString::Compare(const TinyString& str) const throw()
	{
		return Compare(str.STR());
	}
	INT	TinyString::Compare(const string& str) const throw()
	{
		return Compare(str.c_str());
	}
	INT TinyString::Compare(size_t pos, size_t size, const TinyString& str) const throw()
	{
		return Compare(pos, size, str.STR());
	}
	INT TinyString::Compare(const CHAR* s) const  throw()
	{
		if (s != NULL && this->_Mystr != NULL)
			return strcmp(s, this->_Mystr);
		return -1;
	}
	INT TinyString::Compare(size_t pos, size_t size, const CHAR* s) const  throw()
	{
		return Compare(pos, size, s, strlen(s));
	}
	INT TinyString::Compare(size_t pos, size_t size, const CHAR* s, size_t subsize) const  throw()
	{
		ASSERT(s);
		if (this->_Mysize - pos < size)
			size = this->_Mysize - pos;
		CHAR* val = this->_Mystr + pos;
		if (val != NULL)
			return strncmp(val, s, subsize);
		return -1;
	}
	size_t	TinyString::Copy(CHAR* s, size_t pos, size_t size) const
	{
		ASSERT(s);
		ASSERT(this->_Mysize >= pos);
		if (this->_Mysize - pos < size)
			size = this->_Mysize - pos;
		memcpy(s, this->_Mystr + pos, size);
		return size;
	}
	const CHAR& TinyString::GetAt(size_t pos) const
	{
		ASSERT(this->_Mysize >= pos);
		return this->_Mystr[pos];
	}
	void TinyString::Split(char sep, TinyArray<TinyString>& strs) const
	{
		size_t last = 0;
		size_t size = this->_Mysize;
		for (size_t i = 0; i <= size; ++i)
		{
			if (i == size || this->_Mystr[i] == sep)
			{
				size_t s = i - last;
				if (s > 0)
				{
					strs.Add(Substring(last, s).Trim(' '));
					last = i + 1;
				}
			}
		}
	}
	const CHAR& TinyString::operator[](size_t pos)const
	{
		ASSERT(this->_Mysize >= pos);
		return this->_Mystr[pos];
	}
	CHAR& TinyString::operator[](size_t pos)
	{
		ASSERT(this->_Mysize >= pos);
		return this->_Mystr[pos];
	}
	TinyString	TinyString::Trim(const TinyString& str) const
	{
		return Trim(str.STR(), str._Mysize);
	}
	TinyString	TinyString::Trim(const CHAR* s, size_t size) const
	{
		INT _Firstnot = -1;
		INT _Lastnot = -1;
		if (0 < this->_Mysize)
		{
			const CHAR *_Uptr = NULL;
			const CHAR *const _Vptr = this->_Mystr + this->_Mysize;
			for (_Uptr = this->_Mystr; _Uptr < _Vptr; ++_Uptr)
			{
				if (memchr(s, *_Uptr, size) == 0)
				{
					_Firstnot = (_Uptr - this->_Mystr);
					break;
				}
			}
			_Uptr = this->_Mystr + this->_Mysize - 1;
			for (;; --_Uptr)
			{
				if (memchr(s, *_Uptr, size) == 0)
				{
					_Lastnot = (_Uptr - this->_Mystr);
					break;
				}
				else if (_Uptr == this->_Mystr)
					break;
			}
		}
		return TinyString(*this, _Firstnot, _Lastnot - _Firstnot + 1);
	}
	TinyString	TinyString::Trim(CHAR s) const
	{
		return Trim(&s, 1);
	}
	TinyString&	TinyString::Replace(size_t pos, size_t size, const CHAR* s, size_t subsize)
	{
		ASSERT(this->_Mysize >= pos);
		if (this->_Mysize - pos < size)
			size = this->_Mysize - pos;
		size_t _Nm = this->_Mysize - size - pos;
		if (subsize < size)
		{
			memmove(this->_Mystr + pos + subsize,
				this->_Mystr + pos + size,
				_Nm);
		}
		if (0 < subsize || 0 < size)
		{
			size_t Newsize = this->_Mysize + subsize - size;
			if (Newsize < size)
				size = Newsize;
			if (0 < size)
			{
				//»›¡ø≤ª◊„¿©≥‰
				if (this->_Myres < Newsize)
				{
					size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
					size_t _Newres = Newsize | this->_ALLOC_MASK;
					size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
					if (_Maxsize < _Newres)
						_Newres = Newsize;
					else if (_Newres < _size && _size <= _Maxsize)
						_Newres = _size;
					ASSERT(_Newres >= size);
					CHAR *_Ptr = NULL;
					_Ptr = new CHAR[_Newres + 1];
					if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
					{
						memcpy(_Ptr, this->_Mystr, this->_Mysize);
					}
					this->_Myres = _Newres;
					if (size < subsize)
					{
						memmove(_Ptr + pos + subsize, _Ptr + pos + size, _Nm);
					}
					memcpy(_Ptr + pos, s, subsize);
					this->_Mysize = Newsize;
					_Ptr[this->_Mysize] = '\0';
					SAFE_DELETE_ARRAY(this->_Mystr);
					this->_Mystr = _Ptr;
				}
				else//øΩ±¥ƒ⁄¥Ê
				{
					if (size < subsize)
					{
						memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
					}
					memcpy(this->_Mystr + pos, s, subsize);
					this->_Mysize = Newsize;
					this->_Mystr[this->_Mysize] = '\0';
				}
			}
		}
		return (*this);
	}
	TinyString&	TinyString::Replace(size_t pos, size_t size, const CHAR* s)
	{
		ASSERT(s);
		return Replace(pos, size, s, strlen(s));
	}
	TinyString&	TinyString::Replace(size_t pos, size_t size, const TinyString& str)
	{
		return Replace(pos, size, str, 0, str._Mysize);
	}
	TinyString& TinyString::Replace(size_t pos, size_t size, const TinyString& str, size_t subpos, size_t subsize)
	{
		ASSERT(this->_Mysize >= pos || str._Mysize > subpos);
		if (this->_Mysize - pos < size)
			size = this->_Mysize - pos;
		size_t _Num = str._Mysize - subpos;
		if (_Num < subsize)
			subsize = _Num;
		size_t _Nm = this->_Mysize - size - pos;
		size_t _Newsize = this->_Mysize + subsize - size;
		if (this->_Mysize < _Newsize)
		{
			//»›¡ø≤ª◊„¿©≥‰
			if (this->_Myres < _Newsize)
			{
				size_t _Maxsize = (size_t)((size_t)-1 / sizeof(CHAR));
				size_t _Newres = _Newsize | this->_ALLOC_MASK;
				size_t _size = (_Myres * 3) / 2;//¿©≥‰“ª∞Î
				if (_Maxsize < _Newres)
					_Newres = _Newsize;
				else if (_Newres < _size && _size <= _Maxsize)
					_Newres = _size;
				ASSERT(_Newres >= _Newsize);
				CHAR *_Ptr = NULL;
				_Ptr = new CHAR[_Newres + 1];
				if (0 < this->_Mysize)//øΩ±¥¥Ê‘⁄µƒ
				{
					memcpy(_Ptr, this->_Mystr, this->_Mysize);
				}
				this->_Myres = _Newres;
				SAFE_DELETE_ARRAY(this->_Mystr);
				this->_Mystr = _Ptr;
			}
		}
		if (this != &str)
		{
			memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
			memcpy(this->_Mystr + pos, str.STR() + subpos, subsize);
		}
		else if (subsize <= size)
		{
			memmove(this->_Mystr + pos, this->_Mystr + subpos, subsize);
			memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
		}
		else if (subpos <= pos)
		{
			memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
			memmove(this->_Mystr + pos, this->_Mystr + subpos, subsize);
		}
		else if (pos + size <= subpos)
		{
			memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
			memmove(this->_Mystr + pos, this->_Mystr + (subpos + subsize - size), subsize);
		}
		else
		{
			memmove(this->_Mystr + pos, this->_Mystr + subpos, size);
			memmove(this->_Mystr + pos + subsize, this->_Mystr + pos + size, _Nm);
			memmove(this->_Mystr + pos + size, this->_Mystr + subpos + subsize, subsize - size);
		}
		this->_Mysize = _Newsize;
		this->_Mystr[this->_Mysize] = '\0';
		return (*this);
	}
	TinyString	TinyString::Substring(size_t pos, size_t size) const
	{
		return TinyString(*this, pos, size);
	}
	BOOL TinyString::IsEmpty() const throw()
	{
		return (this->_Mysize == 0);
	}
	INT __cdecl TinyString::A2W(CHAR* mbstr, const WCHAR* wcstr, size_t count)
	{
		if (count == 0 && mbstr != NULL)
			return 0;
		INT result = ::WideCharToMultiByte(CP_ACP, 0, wcstr, -1, mbstr, (INT)count, NULL, NULL);
		ASSERT(mbstr == NULL || result <= (INT)count);
		if (result > 0)
		{
			mbstr[result - 1] = 0;
		}
		return result;
	}
	INT __cdecl TinyString::W2A(WCHAR* wcstr, const CHAR* mbstr, size_t count)
	{
		if (count == 0 && wcstr != NULL)
			return 0;
		INT result = ::MultiByteToWideChar(CP_ACP, 0, mbstr, -1, wcstr, (INT)count);
		ASSERT(wcstr == NULL || result <= (INT)count);
		if (result > 0)
		{
			wcstr[result - 1] = 0;
		}
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	static inline void DestructElement(TinyString* ps)
	{
		if (ps != NULL)
		{
			ps->~TinyString();
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	template<typename STR>
	TrimPositions TrimStringT(const STR& input,
		const typename STR::value_type trim_chars[],
		TrimPositions positions,
		STR* output)
	{
		const typename STR::size_type last_char = input.length() - 1;
		const typename STR::size_type first_good_char = (positions&TRIM_LEADING) ?
			input.find_first_not_of(trim_chars) : 0;
		const typename STR::size_type last_good_char = (positions&TRIM_TRAILING) ?
			input.find_last_not_of(trim_chars) : last_char;
		if (input.empty() || (first_good_char == STR::npos) || (last_good_char == STR::npos))
		{
			bool input_was_empty = input.empty();
			output->clear();
			return input_was_empty ? TRIM_NONE : positions;
		}
		*output = input.substr(first_good_char, last_good_char - first_good_char + 1);
		return static_cast<TrimPositions>(
			((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) |
			((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
	}

	BOOL TrimString(const std::string& input,
		const char trim_chars[],
		std::string* output)
	{
		return TrimStringT<std::string>(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
	}
	TrimPositions TrimWhitespaceASCII(const std::string& input,
		TrimPositions positions,
		std::string* output)
	{
		return TrimStringT(input, WhitespaceASCII, positions, output);
	}

	TrimPositions TrimWhitespace(const std::string& input,
		TrimPositions positions,
		std::string* output)
	{
		return TrimWhitespaceASCII(input, positions, output);
	}

	template<typename STR>
	static void SplitStringT(const STR& str,
		const typename STR::value_type s,
		bool trim_whitespace,
		std::vector<STR>* r)
	{
		size_t last = 0;
		size_t i;
		size_t c = str.size();
		for (i = 0; i <= c; ++i)
		{
			if (i == c || str[i] == s)
			{
				size_t len = i - last;
				STR tmp = str.substr(last, len);
				if (trim_whitespace)
				{
					STR t_tmp;
					TrimWhitespace(tmp, TRIM_ALL, &t_tmp);
					r->push_back(t_tmp);
				}
				else
				{
					r->push_back(tmp);
				}
				last = i + 1;
			}
		}
	}

	void SplitString(const std::string& str, char sep, std::vector<std::string>* r)
	{
		SplitStringT(str, sep, true, r);
	}
	void SplitString2(const std::string& str, char sep1, char sep2, std::map<std::string, std::string>& map)
	{
		vector<string> strs;
		SplitString(str, sep1, &strs);
		for (vector<string>::iterator s = strs.begin(); s != strs.end(); ++s)
		{
			vector<string> vals;
			SplitString(*s, sep2, &vals);
			if (vals.size() == 2)
			{
				map[vals[0]] = vals[1];
			}
		}
	}
}

