#pragma once
#include "TinyObject.h"
#include "TinyCollection.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace TinyUI
{
	/// <summary>
	/// �Դ�Сд�޹صķ�ʽ�Ƚ������ַ���s1��s2 ���==0
	/// </summary>
	INT strcasecmp(const char* s1, const char* s2);
	/// <summary>
	/// �Դ�Сд�޹صķ�ʽ�Ƚ������ַ���s1��s2ǰcount���ַ� ���==0
	/// </summary>
	INT strncasecmp(const char* s1, const char* s2, size_t count);
	/// <summary>
	/// Unicode�ַ����Ƚ�
	/// </summary>
	INT strncmp16(const wchar_t* s1, const wchar_t* s2, size_t count);
	/// <summary>
	/// ��װvsnprintf��֤�ַ���ʼ����null��β�������ַ�����, ��ʹ�����ض�ʱҲһ��.
	/// </summary>
	INT vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);
	/// <summary>
	/// ��װvswnprintf��֤�ַ���ʼ����null��β�������ַ�����, ��ʹ�����ض�ʱҲһ��.
	/// </summary>
	INT vswprintf(wchar_t* buffer, size_t size, const wchar_t* format, va_list arguments);
	/// <summary>
	/// �ַ�����ʽ��
	/// </summary>
	string _cdecl StringPrintf(const CHAR* s, ...);
	/// <summary>
	/// ���ֽ��ַ��������ֽ��ַ���
	/// </summary>
	string WStringToString(const wstring str, const DWORD dwType = CP_ACP);
	/// <summary>
	/// ���ֽ��ַ����򵽿��ֽ��ַ���
	/// </summary>
	wstring StringToWString(const string str, const DWORD dwType = CP_ACP);
	/// <summary>
	/// ���ֽ��ַ��������ֽ��ַ���
	/// </summary>
	string WStringToUTF8(const wstring str);
	/// <summary>
	/// ASCIIתUTF8
	/// </summary>
	string ASCIIToUTF8(const string str);
	/// <summary>
	/// UTF8תASCII
	/// </summary>
	string UTF8ToASCII(const string str);
	/// <summary>
	/// UTF8תUTF16
	/// </summary>
	wstring UTF8ToUTF16(const string& s);
	/// <summary>
	/// UTF16תUTF8
	/// </summary>
	string UTF16ToUTF8(const wstring& wide);
	/// <summary>
	/// תСдCHAR
	/// </summary>
	inline CHAR ToLowerASCII(CHAR c)
	{
		return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
	}
	inline CHAR ToUpperASCII(CHAR c)
	{
		return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
	}
	inline BOOL IsAsciiWhitespace(CHAR c)
	{
		return c == ' ' || c == '\r' || c == '\n' || c == '\t';
	}
	inline BOOL IsAsciiAlpha(CHAR c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}
	inline BOOL IsAsciiUpper(CHAR c)
	{
		return c >= 'A' && c <= 'Z';
	}
	inline BOOL IsAsciiLower(CHAR c)
	{
		return c >= 'a' && c <= 'z';
	}
	inline BOOL IsAsciiDigit(CHAR c)
	{
		return c >= '0' && c <= '9';
	}
	inline BOOL IsHexDigit(CHAR c)
	{
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}
	inline string ToLowerASCII(const string& str)
	{
		string val;
		val.reserve(str.size());
		for (size_t i = 0; i < val.size(); i++)
		{
			val.push_back(ToLowerASCII(str[i]));
		}
		return val;
	};
	inline string ToUpperASCII(const string& str)
	{
		string val;
		val.reserve(str.size());
		for (size_t i = 0; i < val.size(); i++)
		{
			val.push_back(ToUpperASCII(str[i]));
		}
		return val;
	};
	static void StringAppendVT(string* dst, const CHAR* format, va_list ap);
	void StringAppendF(string* dst, const CHAR* format, ...);
	/// <summary>
	/// �ַ�����������ֽ�
	/// </summary>
	class TinyString
	{
	public:
		TinyString();
		explicit TinyString(size_t size);
		TinyString(TinyString&& s);
		TinyString(const CHAR* s);
		TinyString(const TinyString& s);
		TinyString(const TinyString& s, size_t pos, size_t size);
		TinyString(const string& s);
		~TinyString();
	public:
		TinyString& Assign(const CHAR* s);
		TinyString& Assign(const CHAR* s, size_t size);
		TinyString& Assign(const TinyString& str, size_t pos, size_t size);
		TinyString& Assign(const string& s);
		TinyString&	Append(const CHAR* s);
		TinyString&	Append(const CHAR* s, size_t size);
		TinyString& Append(const TinyString& str, size_t pos, size_t size);
		TinyString& Append(const TinyString& str);
		TinyString& operator+= (const TinyString& str);
		TinyString& operator+= (const CHAR* s);
		TinyString& operator+= (CHAR s);
		TinyString& operator=(TinyString&& str);
		TinyString& operator= (const TinyString& str);
		TinyString& operator= (const CHAR* s);
		TinyString& operator= (CHAR s);
		BOOL		operator==(const TinyString& str);
		BOOL		operator==(const TinyString& str) const;
		BOOL		operator==(const CHAR* s);
		BOOL		operator==(const CHAR* s) const;
		BOOL		operator==(TinyString& str);
		BOOL		operator==(CHAR* s);
		BOOL		operator==(const string& str);
		BOOL		operator==(const string& str) const;
		BOOL		operator==(string& str);
		BOOL		operator!=(const TinyString& str);
		BOOL		operator!=(const CHAR* s);
		BOOL		operator!=(TinyString& str);
		BOOL		operator!=(CHAR* s);
		BOOL		operator!=(const string& str);
		BOOL		operator!=(string& str);
		void		Resize(size_t _Newsize);
		TinyString& Erase(size_t pos);
		TinyString& Erase(size_t pos, size_t size);
		TinyString& Insert(size_t pos, const CHAR* s);
		TinyString& Insert(size_t pos, const CHAR* s, size_t subsize);
		TinyString& Insert(size_t pos, const TinyString& str);
		TinyString& Insert(size_t pos, const TinyString& str, size_t subpos, size_t subsize);
		INT			IndexOf(const CHAR* s, size_t pos = 0) const;
		INT			IndexOf(const TinyString& str, size_t pos = 0) const;
		INT			IndexOf(const CHAR* s, size_t pos, size_t size) const;
		INT			IndexOf(CHAR s, size_t pos = 0) const;
		INT			LastIndexOf(const CHAR* s, size_t pos = -1) const;
		INT			LastIndexOf(const TinyString& str, size_t pos = -1) const;
		INT			LastIndexOf(const CHAR* s, size_t pos, size_t size) const;
		INT			LastIndexOf(CHAR s, size_t pos = -1) const;
		INT			Compare(const TinyString& str) const throw();
		INT			Compare(const string& str) const throw();
		INT			Compare(size_t pos, size_t size, const TinyString& str) const throw();
		INT			Compare(const CHAR* s) const  throw();
		INT			Compare(size_t pos, size_t size, const CHAR* s) const  throw();
		INT			Compare(size_t pos, size_t size, const CHAR* s, size_t count) const  throw();
		size_t		Copy(CHAR* s, size_t pos, size_t size) const;
		TinyString&	Replace(size_t pos, size_t size, const TinyString& str);
		TinyString& Replace(size_t pos, size_t size, const TinyString& str, size_t subpos, size_t subsize);
		TinyString&	Replace(size_t pos, size_t size, const CHAR* s);
		TinyString&	Replace(size_t pos, size_t size, const CHAR* s, size_t subsize);
		TinyString	Substring(size_t pos, size_t size) const;//RVO�Ż�
		const CHAR& GetAt(size_t pos) const;
		void		Split(char sep, TinyArray<TinyString>& strs) const;
		TinyString	Trim(const TinyString& str) const;
		TinyString	Trim(const CHAR* s, size_t size) const;
		TinyString	Trim(CHAR s) const;
		BOOL		IsEmpty() const throw();
		const CHAR& operator[](size_t pos)const;
		CHAR&		operator[] (size_t pos);
		size_t		GetSize() const;
		CHAR*		STR() const;
		const CHAR*	CSTR() const;
		wstring		ToWString() const;
		string		ToUTF8() const;
		static TinyString _cdecl Format(const CHAR* s, ...);
		static INT __cdecl A2W(CHAR* mbstr, const WCHAR* wcstr, size_t count);
		static INT __cdecl W2A(WCHAR* wcstr, const CHAR* mbstr, size_t count);
	private:
		enum
		{
			_ALLOC_MASK = 15
		};
	protected:
		CHAR*	_Mystr;
		size_t	_Mysize;
		size_t	_Myres;
	};

	template<>
	class DefaultTraits < TinyString >
	{
	public:
		static INT  Compare(const TinyString& value1, const TinyString& value2)
		{
			return value1.Compare(value2);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	enum TrimPositions
	{
		TRIM_NONE = 0,
		TRIM_LEADING = 1 << 0,
		TRIM_TRAILING = 1 << 1,
		TRIM_ALL = TRIM_LEADING | TRIM_TRAILING,
	};
	const char WhitespaceASCII[] =
	{
		0x09,
		0x0A,
		0x0B,
		0x0C,
		0x0D,
		0x20,
		0
	};
	template<typename STR>
	TrimPositions TrimStringT(const STR& input, const typename STR::value_type trim_chars[], TrimPositions positions, STR* output);
	BOOL TrimString(const std::string& input, const char trim_chars[], std::string* output);
	TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions, std::string* output);
	TrimPositions TrimWhitespace(const std::string& input, TrimPositions positions, std::string* output);
	template<typename STR>
	static void SplitStringT(const STR& str, const typename STR::value_type s, bool trim_whitespace, std::vector<STR>* r);
	void SplitString(const std::string& str, char sep, std::vector<std::string>* r);
	void SplitString2(const std::string& str, char sep1, char sep2, std::map<std::string, std::string>& map);
}



