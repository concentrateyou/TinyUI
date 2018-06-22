#pragma once
#include "TinyObject.h"
#include "TinyCollection.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace TinyFramework
{
	/// <summary>
	/// 以大小写无关的方式比较两个字符串s1和s2 相等==0
	/// </summary>
	INT strcasecmp(const char* s1, const char* s2);
	/// <summary>
	/// 以大小写无关的方式比较两个字符串s1和s2前count个字符 相等==0
	/// </summary>
	INT strncasecmp(const char* s1, const char* s2, size_t count);
	/// <summary>
	/// Unicode字符串比较
	/// </summary>
	INT strncmp16(const wchar_t* s1, const wchar_t* s2, size_t count);
	/// <summary>
	/// 封装vsnprintf保证字符串始终以null结尾并返回字符长度, 即使发生截断时也一样.
	/// </summary>
	INT vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);
	/// <summary>
	/// 封装vswnprintf保证字符串始终以null结尾并返回字符长度, 即使发生截断时也一样.
	/// </summary>
	INT vswprintf(wchar_t* buffer, size_t size, const wchar_t* format, va_list arguments);
	/// <summary>
	/// 字符串格式化
	/// </summary>
	string _cdecl StringPrintf(const CHAR* s, ...);
	/// <summary>
	/// 宽字节字符创到多字节字符串
	/// </summary>
	string WStringToString(const wstring& str, const DWORD dwType = CP_ACP);
	/// <summary>
	/// 多字节字符串打到宽字节字符串
	/// </summary>
	wstring StringToWString(const string& str, const DWORD dwType = CP_ACP);
	/// <summary>
	/// 宽字节字符创到多字节字符串
	/// </summary>
	string WStringToUTF8(const wstring& str);
	/// <summary>
	/// ASCII转UTF8
	/// </summary>
	string ASCIIToUTF8(const string& str);
	/// <summary>
	/// UTF8转ASCII
	/// </summary>
	string UTF8ToASCII(const string& str);
	/// <summary>
	/// UTF8转UTF16
	/// </summary>
	wstring UTF8ToUTF16(const string& s);
	/// <summary>
	/// UTF16转UTF8
	/// </summary>
	string UTF16ToUTF8(const wstring& wide);
	/// <summary>
	/// 转小写CHAR
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
	/// 字符串操作类多字节
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
		TinyString	Substring(size_t pos, size_t size) const;//RVO优化
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

	/// <summary>
	/// 字符串Map
	/// </summary>
	template<class T>
	class TinyStringMap
	{
		DISALLOW_COPY_AND_ASSIGN(TinyStringMap)
	protected:
		struct TinyNode : public __ITERATOR
		{
			TinyNode*	m_pNext;
			UINT		m_hashValue;
			TinyString	m_key;
			T			m_value;
		public:
			TinyNode(T&& value)
				:m_value(std::move(value)),
				m_pNext(NULL)
			{

			}
			TinyNode(const T& value)
				:m_value(value),
				m_pNext(NULL)
			{

			}
		};
	public:
		explicit TinyStringMap(INT_PTR blockSize = 10);
		virtual ~TinyStringMap();
		INT_PTR		GetSize() const;
		BOOL		IsEmpty() const;
		BOOL		Lookup(LPCTSTR key, T& value) const;
		T&			operator[](LPCTSTR key);
		void		SetAt(LPCTSTR key, const T& value);
		void		SetAt(LPCTSTR key, T&& value);
		BOOL		Remove(LPCTSTR key);
		void		RemoveAll();
		void		Initialize(UINT hashSize);
	private:
		TinyNode*	New(const T& value);
		TinyNode*	New(T&& value);
		void		Delete(TinyNode* ps);
		TinyNode*	Lookup(LPCTSTR key, UINT& index, UINT& hash) const;
		UINT		HashKey(LPCTSTR key) const;
	private:
		UINT		m_hashSize;
		INT_PTR		m_blockSize;
		INT_PTR		m_count;
		TinyNode**	m_pTable;
		TinyNode*	m_pFreeList;
		TinyPlex*	m_pBlocks;
	};
	template<class T>
	TinyStringMap<T>::TinyStringMap(INT_PTR blockSize)
		:m_blockSize(blockSize),
		m_hashSize(17),
		m_count(0),
		m_pTable(NULL),
		m_pFreeList(NULL),
		m_pBlocks(NULL)
	{
		m_blockSize = m_blockSize == 0 ? 10 : m_blockSize;
	}
	template<class T>
	TinyStringMap<T>::~TinyStringMap()
	{
		RemoveAll();
	}
	template<class T>
	INT_PTR	TinyStringMap<T>::GetSize() const
	{
		return m_count;
	}
	template<class T>
	BOOL TinyStringMap<T>::IsEmpty() const
	{
		return m_count == 0;
	}
	template<class T>
	void TinyStringMap<T>::Initialize(UINT hashSize)
	{
		if (hashSize == 0)
			hashSize = 17;
		SAFE_DELETE_ARRAY(m_pTable);
		m_pTable = new TinyNode*[hashSize];
		memset(m_pTable, 0, sizeof(TinyNode*) * hashSize);
		m_hashSize = hashSize;
	}
	template<class T>
	UINT TinyStringMap<T>::HashKey(LPCTSTR key) const
	{
		//STL算法xfunctional
		UINT value = 2166136261U;
		UINT uFirst = 0;
		UINT uLast = (UINT)_tcslen(key);
		UINT uStride = 1 + uLast / 10;
		for (; uFirst < uLast; uFirst += uStride)
		{
			value = 16777619U * value ^ (UINT)key[uFirst];
		}
		return(value);
	}
	template<class T>
	BOOL TinyStringMap<T>::Lookup(LPCTSTR key, T& value) const
	{
		UINT bucket = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		ps = Lookup(key, bucket, hash);
		if (ps == NULL)
			return FALSE;
		value = ps->m_value;
		return TRUE;
	}
	template<class T>
	T& TinyStringMap<T>::operator[](LPCTSTR key)
	{
		UINT bucket = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, bucket, hash)) == NULL)
		{
			if (m_pTable == NULL)
			{
				Initialize(m_hashSize);
			}
			ps = New(T());
			ps->m_hashValue = hash;
			ps->m_key = key;
			ps->m_pNext = m_pTable[bucket];
			m_pTable[bucket] = ps;
		}
		return ps->m_value;
	}
	template<class T>
	void TinyStringMap<T>::SetAt(LPCTSTR key, const T& value)
	{
		(*this)[key] = value;
	}
	template<class T>
	void TinyStringMap<T>::SetAt(LPCTSTR key, T&& value)
	{
		(*this)[key] = value;
	}
	template<class T>
	BOOL TinyStringMap<T>::Remove(LPCTSTR key)
	{
		if (m_pTable == NULL)
			return FALSE;
		TinyNode** ppPrev = NULL;
		UINT hash = 0;
		hash = HashKey(key);
		ppPrev = &m_pTable[hash % m_hashSize];
		TinyNode* ps = NULL;
		for (ps = *ppPrev; ps != NULL; ps = ps->m_pNext)
		{
			if ((ps->m_hashValue == hash) && (ps->m_key == key))
			{
				*ppPrev = ps->m_pNext;
				Delete(ps);
				return TRUE;
			}
			ppPrev = &ps->m_pNext;
		}
		return FALSE;
	}
	template<class T>
	void TinyStringMap<T>::RemoveAll()
	{
		if (m_pTable != NULL)
		{
			for (UINT index = 0; index < m_hashSize; index++)
			{
				TinyNode* ps = NULL;
				for (ps = m_pTable[index]; ps != NULL; ps = ps->m_pNext)
				{
					ps->~TinyNode();
				}
			}
			SAFE_DELETE_ARRAY(m_pTable);
		}
		m_count = 0;
		m_pFreeList = NULL;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
	}
	template<class T>
	typename TinyStringMap<T>::TinyNode* TinyStringMap<T>::New(T&& value)
	{
		if (m_pFreeList == NULL)
		{
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_blockSize, sizeof(TinyNode));
			if (pPlex == NULL)
				return NULL;
			TinyNode* ps = static_cast<TinyNode*>(pPlex->data());
			ps += m_blockSize - 1;
			for (INT_PTR iBlock = m_blockSize - 1; iBlock >= 0; iBlock--)
			{
				ps->m_pNext = m_pFreeList;
				m_pFreeList = ps;
				ps--;
			}
		}
		TinyNode* pNew = m_pFreeList;
		m_pFreeList = m_pFreeList->m_pNext;
		m_count++;
		ASSERT(m_count > 0);
#pragma push_macro("new")
#undef new
		new(pNew) TinyNode(std::move(value));
#pragma pop_macro("new")
		return pNew;
	}
	template<class T>
	typename TinyStringMap<T>::TinyNode* TinyStringMap<T>::New(const T& value)
	{
		if (m_pFreeList == NULL)
		{
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_blockSize, sizeof(TinyNode));
			if (pPlex == NULL)
				return NULL;
			TinyNode* ps = static_cast<TinyNode*>(pPlex->data());
			ps += m_blockSize - 1;
			for (INT_PTR iBlock = m_blockSize - 1; iBlock >= 0; iBlock--)
			{
				ps->m_pNext = m_pFreeList;
				m_pFreeList = ps;
				ps--;
			}
		}
		TinyNode* pNew = m_pFreeList;
		m_pFreeList = m_pFreeList->m_pNext;
		m_count++;
		ASSERT(m_count > 0);
#pragma push_macro("new")
#undef new
		new(pNew) TinyNode(value);
#pragma pop_macro("new")
		return pNew;
	}
	template<class T>
	void TinyStringMap<T>::Delete(TinyNode* ps)
	{
		ps->~TinyNode();
		ps->m_pNext = m_pFreeList;
		m_pFreeList = ps;
		m_count--;
		ASSERT(m_count >= 0);
		if (m_count == 0)
		{
			RemoveAll();
		}
	}
	template<class T>
	typename TinyStringMap<T>::TinyNode* TinyStringMap<T>::Lookup(LPCTSTR key, UINT& bucket, UINT& hash) const
	{
		hash = HashKey(key);
		bucket = hash % m_hashSize;
		if (m_pTable == NULL)
			return NULL;
		TinyNode* ps = NULL;
		for (ps = m_pTable[bucket]; ps != NULL; ps = ps->m_pNext)
		{
			if (ps->m_key == key && ps->m_hashValue == hash)
			{
				return ps;
			}
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
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



