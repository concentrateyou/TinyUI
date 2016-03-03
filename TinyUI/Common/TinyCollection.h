#pragma once
#include "TinyCommon.h"
#include <math.h>

namespace TinyUI
{
	/// <summary>
	/// T是对象调用拷贝构造函数,否则直接赋值
	/// </summary>
	template<class T>
	class TinyPlaceNew
	{
	public:
		TinyPlaceNew(const T& _myT) :
			m_myT(_myT)
		{
		};
		template <class _Ty>
		void * __cdecl operator new(size_t, _Ty* p)
		{
			return p;
		};
		template <class _Ty>
		void __cdecl operator delete(void*, _Ty*)
		{
		};
		T m_myT;
	};
	/// <summary>
	/// 简单的Map, Key-Value一一对应,内存中对齐排列
	/// <summary>
	template<class K, class V>
	class TinySimpleMap
	{
	public:
		TinySimpleMap();
		~TinySimpleMap();
		BOOL Add(const K& myKey, const V& myValue);
		BOOL Remove(const K& myKey);
		BOOL RemoveAt(INT index);
		void RemoveAll();
		INT	 GetSize() const;
		INT	 FindKey(const K& myKey) const;
		INT  FindValue(const V& myValue) const;
		K&	 GetKeyAt(INT index);
		V&	 GetValueAt(INT index);
		V*	 Lookup(const K& key) const;
		V*	 Lookup(K& key);
		V&	 operator[](const K& myKey) const;
		V&	 operator[](K& myKey);
	private:
		INT		m_size;
		K*		m_myKey;
		V*		m_myValue;
	};
	template<class K, class V>
	TinySimpleMap<K, V>::TinySimpleMap()
		:m_myKey(NULL),
		m_myValue(NULL),
		m_size(0)
	{

	}
	template<class K, class V>
	TinySimpleMap<K, V>::~TinySimpleMap()
	{
		RemoveAll();
	}
	template<class K, class V>
	INT	 TinySimpleMap<K, V>::GetSize() const
	{
		return m_size;
	}
	template<class K, class V>
	BOOL TinySimpleMap<K, V>::Add(const K& myKey, const V& myValue)
	{
		K* _myKey = NULL;
		_myKey = (K*)_recalloc(m_myKey, m_size + 1, sizeof(K));
		if (_myKey == NULL)
		{
			return FALSE;
		}
		m_myKey = _myKey;
		new(m_myKey + m_size) TinyPlaceNew<K>(myKey);
		V* _myValue = NULL;
		_myValue = (V*)_recalloc(m_myValue, m_size + 1, sizeof(V));
		if (_myValue == NULL)
		{
			return FALSE;
		}
		m_myValue = _myValue;
#pragma push_macro("new")
#undef new
		::new(m_myValue + m_size) TinyPlaceNew<V>(myValue);
#pragma pop_macro("new")
		m_size++;
		return TRUE;
	}
	template<class K, class V>
	BOOL TinySimpleMap<K, V>::Remove(const K& myKey)
	{
		INT index = FindKey(myKey);
		if (index < 0)
		{
			return FALSE;
		}
		return RemoveAt(index);
	}
	template<class K, class V>
	INT TinySimpleMap<K, V>::FindKey(const K& myKey) const
	{
		for (INT i = 0; i < m_size; i++)
		{
			if (m_myKey[i] == myKey)
			{
				return i;
			}
		}
		return -1;
	}
	template<class K, class V>
	INT TinySimpleMap<K, V>::FindValue(const V& myValue) const
	{
		for (INT i = 0; i < m_size; i++)
		{
			if (m_myValue[i] == myValue)
			{
				return i;
			}
		}
		return -1;
	}
	template<class K, class V>
	V* TinySimpleMap<K, V>::Lookup(const K& key) const
	{
		INT index = FindKey(key);
		if (index < 0)
		{
			return NULL;
		}
		return &m_myValue[index];
	}
	template<class K, class V>
	V* TinySimpleMap<K, V>::Lookup(K& key)
	{
		INT index = FindKey(key);
		if (index < 0)
		{
			return NULL;
		}
		return &m_myValue[index];
	}
	template<class K, class V>
	K& TinySimpleMap<K, V>::GetKeyAt(INT index)
	{
		if (index < 0 || index >= m_size)
			throw("无效的index");
		return m_myKey[index];
	}
	template<class K, class V>
	V& TinySimpleMap<K, V>::GetValueAt(INT index)
	{
		if (index < 0 || index >= m_size)
			throw("无效的index");
		return m_myValue[index];
	}
	template<class K, class V>
	BOOL TinySimpleMap<K, V>::RemoveAt(INT index)
	{
		if (index < 0 || index >= m_size)
		{
			return FALSE;
		}
		m_myKey[index].~K();
		m_myValue[index].~V();
		//移动内存
		if (index != (m_size - 1))
		{
			memmove_s((void*)(m_myKey + index), (m_size - index) * sizeof(K), (void*)(m_myKey + index + 1), (m_size - (index + 1)) * sizeof(K));
			memmove_s((void*)(m_myValue + index), (m_size - index) * sizeof(V), (void*)(m_myValue + index + 1), (m_size - (index + 1)) * sizeof(V));
		}
		//重新分配内存 
		K* myKey;
		myKey = (K*)_recalloc(m_myKey, (m_size - 1), sizeof(K));
		if (myKey != NULL || m_size == 1)
		{
			m_myKey = myKey;
		}
		V* myValue;
		myValue = (V*)_recalloc(m_myValue, (m_size - 1), sizeof(V));
		if (myValue != NULL || m_size == 1)
		{
			m_myValue = myValue;
		}
		m_size--;
		return TRUE;
	}
	template<class K, class V>
	void TinySimpleMap<K, V>::RemoveAll()
	{
		if (m_myKey != NULL)
		{
			for (INT i = 0; i < m_size; i++)
			{
				m_myKey[i].~K();
				m_myValue[i].~V();
			}
			SAFE_FREE(m_myKey);
		}
		if (m_myValue != NULL)
		{
			SAFE_FREE(m_myValue);
		}
		m_size = 0;
	}
	template<class K, class V>
	V& TinySimpleMap<K, V>::operator[](const K& myKey) const
	{
		INT index = FindKey(myKey);
		if (index < 0) throw("无效的Key");
		return m_myValue[index];
	}
	template<class K, class V>
	V& TinySimpleMap<K, V>::operator[](K& myKey)
	{
		INT index = FindKey(myKey);
		if (index < 0) throw("无效的Key");
		return m_myValue[index];
	}
	/// <summary>
	/// 简单的动态数组
	/// <summary>
	template<class T>
	class TinyArray
	{
	public:
		TinyArray();
		TinyArray(const TinyArray<T>& myT);
		TinyArray<T>& operator=(const TinyArray<T>& myT);
		~TinyArray();
		BOOL		IsEmpty() const;
		BOOL		Add(const T& myT);
		BOOL		Insert(INT index, const T& myT);
		BOOL		Remove(const T& myT);
		BOOL		RemoveAt(INT index);
		void		RemoveAll();
		INT			Lookup(const T& myT) const;
		const T*	GetDate() const;
		T*			GetDate();
		INT			GetSize() const;
		const T&	operator[](INT index) const;
		T&			operator[](INT index);
	protected:
		T*	m_value;
		INT m_size;
		INT m_alloc_size;
	};
	template<class T>
	TinyArray<T>::TinyArray()
		:m_value(NULL),
		m_size(0),
		m_alloc_size(0)
	{

	}
	template<class T>
	TinyArray<T>::TinyArray(const TinyArray<T>& myT)
		:m_value(NULL),
		m_size(0),
		m_alloc_size(0)
	{
		INT size = myT.GetSize();
		if (size > 0)
		{
			m_value = (T*)calloc(size, sizeof(T));
			if (m_value != NULL)
			{
				m_alloc_size = size;
				for (INT i = 0; i < size; i++)
				{
					Add(myT[i]);
				}
			}
		}
	}
	template<class T>
	TinyArray<T>& TinyArray<T>::operator=(const TinyArray<T>& myT)
	{
		INT size = myT.GetSize();
		if (m_size != size)
		{
			RemoveAll();
			m_value = (T*)calloc(size, sizeof(T));
			if (m_value != NULL)
			{
				m_alloc_size = m_size;
			}
		}
		else
		{
			for (INT i = m_size; i > 0; i--)
			{
				RemoveAt(i - 1);
			}
		}
		for (INT i = 0; i < size; i++)
		{
			Add(myT[i]);
		}
		return *this;
	}
	template<class T>
	TinyArray<T>::~TinyArray()
	{
		RemoveAll();
	}
	template<class T>
	const T*  TinyArray<T>::GetDate() const
	{
		return m_value;
	}
	template<class T>
	T*	TinyArray<T>::GetDate()
	{
		return m_value;
	}
	template<class T>
	INT	TinyArray<T>::GetSize() const
	{
		return m_size;
	}
	template<class T>
	BOOL TinyArray<T>::IsEmpty() const
	{
		return m_size == 0;
	}
	template<class T>
	BOOL TinyArray<T>::Add(const T& myT)
	{
		if (m_size == m_alloc_size)//需要重新分配内存
		{
			T* _myP = NULL;
			INT size = (m_alloc_size == 0) ? 1 : (m_size * 2);
			if (size < 0 || size >(INT_MAX / sizeof(T)))
			{
				return FALSE;
			}
			_myP = (T*)_recalloc(m_value, size, sizeof(T));
			if (_myP == NULL)
			{
				return FALSE;
			}
			m_alloc_size = size;
			m_value = _myP;
		}
#pragma push_macro("new")
#undef new
		::new(m_value + m_size) TinyPlaceNew<T>(myT);
#pragma pop_macro("new")
		m_size++;
		return TRUE;
	}
	template<class T>
	BOOL TinyArray<T>::Remove(const T& myT)
	{
		INT index = Lookup(myT);
		if (index == -1)
		{
			return FALSE;
		}
		return RemoveAt(index);
	}
	template<class T>
	BOOL TinyArray<T>::RemoveAt(INT index)
	{
		if (index < 0 || index >= m_size)
		{
			return FALSE;
		}
		m_value[index].~T();
		if (index != (m_size - 1))
		{
			//移动内存
			memmove_s((void*)(m_value + index),
				(m_size - index) * sizeof(T),
				(void*)(m_value + index + 1),
				(m_size - (index + 1)) * sizeof(T));
		}
		m_size--;
		return TRUE;
	}
	template<class T>
	void TinyArray<T>::RemoveAll()
	{
		if (m_value != NULL)
		{
			for (INT i = 0; i < m_size; i++)
			{
				m_value[i].~T();
			}
			SAFE_FREE(m_value);
		}
		m_size = 0;
		m_alloc_size = 0;
	}
	template<class T>
	BOOL TinyArray<T>::Insert(INT index, const T& myT)
	{
		//Index超出范围
		if (index < 0 || index > m_size)
		{
			return FALSE;
		}
		//需要重新分配内存
		if (m_size == m_alloc_size)
		{
			T* _myP = NULL;
			INT size = (m_alloc_size == 0) ? 1 : (m_size * 2);
			if (size < 0 || size >(INT_MAX / sizeof(T)))
			{
				return FALSE;
			}
			_myP = (T*)_recalloc(m_value, size, sizeof(T));
			if (_myP == NULL)
			{
				return FALSE;
			}
			m_alloc_size = size;
			m_value = _myP;
		}
		//向后移动一个T大小内存
		memmove_s((void*)(m_value + index + 1),
			(m_size - index) * sizeof(T),
			(void*)(m_value + index),
			(m_size - index) * sizeof(T));
#pragma push_macro("new")
#undef new
		::new(m_value + index) TinyPlaceNew<T>(myT);
#pragma pop_macro("new")
		m_size++;
		return TRUE;
	}
	template<class T>
	INT	TinyArray<T>::Lookup(const T& myT) const
	{
		for (INT i = 0; i < m_size; i++)
		{
			if (m_value[i] == myT)
			{
				return i;
			}
		}
		return -1;
	}
	template<class T>
	const T& TinyArray<T>::operator[](INT index) const
	{
		if (index < 0 || index >= m_size)
			throw("无效的index");
		return m_value[index];
	}
	template<class T>
	T&	TinyArray<T>::operator[](INT index)
	{
		if (index < 0 || index >= m_size)
			throw("无效的index");
		return m_value[index];
	}
	/// <summary>
	/// 内存池
	/// </summary>
#pragma pack(push, 4)
	struct NO_VTABLE TinyPlex
	{
		TinyPlex* pNext;
		void* data() { return this + 1; }
		static TinyPlex* PASCAL Create(TinyPlex*& ps, UINT_PTR nMax, UINT_PTR nElementSize);
		void Destory();
	};
#pragma pack(pop)
	/// <summary>
	/// 迭代器的内部指针
	/// </summary>
	struct __ITERATOR{};
	typedef __ITERATOR* ITERATOR;
	/// <summary>
	/// 类型萃取器
	/// </summary>
	template<typename T>
	class DefaultTraits
	{
	public:
		static INT  Compare(const T& value1, const T& value2)
		{
			if (value1 < value2)
				return (-1);
			else if (value1 == value2)
				return (0);
			else
				return (1);
		}
	};
	/// <summary>
	/// 链表结构
	/// </summary>
	template<typename T, typename Traits = DefaultTraits<T>>
	class TinyLinkList
	{
		DISALLOW_COPY_AND_ASSIGN(TinyLinkList)
	private:
		class TinyEntry : public __ITERATOR
		{
		public:
			T			m_value;
			TinyEntry*	m_pNext;
			TinyEntry*	m_pPrev;
		public:
			TinyEntry(const T& value)
				:m_value(value)
			{

			}
		};
	public:
		explicit TinyLinkList(DWORD dwSize = 10);
		~TinyLinkList();
		ITERATOR operator[](const T& value) const;
		DWORD	GetSize() const;
		BOOL	IsEmpty() const;
		ITERATOR InsertFirst(const T& value);
		ITERATOR InsertLast(const T& value);
		ITERATOR InsertBefore(ITERATOR pos, const T& value);
		ITERATOR InsertAfter(ITERATOR pos, const T& value);
		ITERATOR Lookup(const T& value, ITERATOR pos = NULL) const throw();
		ITERATOR First() const;
		ITERATOR Last() const;
		ITERATOR Next(ITERATOR pos) const;
		ITERATOR Prev(ITERATOR pos) const;
		T& GetAt(ITERATOR pos);
		const T& GetAt(ITERATOR pos) const throw();
		BOOL RemoveAt(ITERATOR pos) throw();
		void RemoveAll();
	private:
		typename TinyLinkList<T, Traits>::TinyEntry* New(TinyEntry* pPrev, TinyEntry* pNext, const T& value);
		BOOL Delete(TinyEntry* ps);
	private:
		DWORD		m_dwBlockSize;
		DWORD		m_dwCount;
		TinyEntry*	m_pFirst;
		TinyEntry*	m_pLast;
		TinyEntry*  m_pEntry;
		TinyPlex*	m_pBlocks;
	};
	template<typename T, typename Traits>
	TinyLinkList<T, Traits>::~TinyLinkList()
	{
		RemoveAll();
	}
	template<typename T, typename Traits>
	TinyLinkList<T, Traits>::TinyLinkList(DWORD dwBlockSize)
		:m_dwBlockSize(dwBlockSize),
		m_dwCount(0),
		m_pFirst(NULL),
		m_pLast(NULL),
		m_pEntry(NULL),
		m_pBlocks(NULL)
	{

	}
	template<typename T, typename Traits>
	typename TinyLinkList<T, Traits>::TinyEntry* TinyLinkList<T, Traits>::New(TinyEntry* pPrev, TinyEntry* pNext, const T& value)
	{
		if (m_pEntry == NULL)
		{
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_dwBlockSize, sizeof(TinyEntry));
			if (pPlex == NULL) return NULL;
			TinyEntry* ps = static_cast<TinyEntry*>(pPlex->data());
			ps += m_dwBlockSize - 1;
			for (INT_PTR iBlock = m_dwBlockSize - 1; iBlock >= 0; iBlock--)
			{
				ps->m_pNext = m_pEntry;
				m_pEntry = ps;
				ps--;
			}
		}
		TinyEntry* pNew = m_pEntry;
		m_pEntry = m_pEntry->m_pNext;
		pNew->m_pPrev = pPrev;
		pNew->m_pNext = pNext;
		m_dwCount++;
		::new(pNew)TinyEntry(value);
		return(pNew);
	}
	template<typename T, typename Traits>
	BOOL TinyLinkList<T, Traits>::Delete(TinyEntry* ps)
	{
		if (!ps) return FALSE;
		ps->~TinyEntry();
		ps->m_pNext = m_pEntry;
		m_pEntry = ps;
		m_dwCount--;
		return TRUE;
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList<T, Traits>::InsertFirst(const T& value)
	{
		TinyEntry* ps = New(NULL, m_pFirst, value);
		if (m_pFirst != NULL)
			m_pFirst->m_pPrev = ps;
		else
			m_pLast = ps;
		m_pFirst = ps;
		return(ITERATOR(ps));
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList<T, Traits>::InsertLast(const T& value)
	{
		TinyEntry* ps = New(m_pLast, NULL, value);
		if (m_pLast != NULL)
			m_pLast->m_pNext = ps;
		else
			m_pFirst = ps;
		m_pLast = ps;
		return(ITERATOR(ps));
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList<T, Traits>::InsertBefore(ITERATOR pos, const T& value)
	{
		if (pos == NULL)
			return InsertFirst(value);
		TinyEntry* pOld = static_cast<TinyEntry*>(pos);
		TinyEntry* pNew = New(pOld->m_pPrev, pOld, value);
		if (pOld->m_pPrev != NULL)
			pOld->m_pPrev->m_pNext = pNew;
		else
			m_pFirst = pNew;
		pOld->m_pPrev = pNew;
		return(ITERATOR(pNew));
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList<T, Traits>::InsertAfter(ITERATOR pos, const T& value)
	{
		if (pos == NULL)
			return InsertLast(value);
		TinyEntry* pOld = static_cast<TinyEntry*>(pos);
		TinyEntry* pNew = New(value, pOld, pOld->m_pNext);
		if (pOld->m_pNext != NULL)
			pOld->m_pNext->m_pPrev = pNew;
		else
			m_pLast = pNew;
		pOld->m_pNext = pNew;
		return(ITERATOR(pNew));
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList<T, Traits>::Lookup(const T& value, ITERATOR pos) const throw()
	{
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		if (ps == NULL)
			ps = m_pFirst;
		else
			ps = ps->m_pNext;
		for (; ps != NULL; ps = ps->m_pNext)
		{
			if (Traits::Compare(ps->m_value, value) == 0)
				return(ITERATOR(ps));
		}
		return(NULL);
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList< T, Traits>::First() const
	{
		return ITERATOR(m_pFirst);
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList< T, Traits>::Last() const
	{
		return ITERATOR(m_pLast);
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList< T, Traits>::Next(ITERATOR pos) const
	{
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		if (ps != NULL)
		{
			return ITERATOR(ps->m_pNext);
		}
		return NULL;
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList< T, Traits>::Prev(ITERATOR pos) const
	{
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		if (ps != NULL)
		{
			return ITERATOR(ps->m_pPrev);
		}
		return NULL;
	}
	template<typename T, typename Traits>
	const T& TinyLinkList< T, Traits>::GetAt(ITERATOR pos) const throw()
	{
		ASSERT(pos);
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		return ps->m_value;
	}
	template<typename T, typename Traits>
	T& TinyLinkList< T, Traits>::GetAt(ITERATOR pos)
	{
		ASSERT(pos);
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		return ps->m_value;
	}
	template<typename T, typename Traits>
	BOOL TinyLinkList< T, Traits>::RemoveAt(ITERATOR pos) throw()
	{
		if (!pos) return FALSE;
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		if (ps == m_pFirst)
			m_pFirst = ps->m_pNext;
		else
			ps->m_pPrev->m_pNext = ps->m_pNext;
		if (ps == m_pLast)
			m_pLast = ps->m_pPrev;
		else
			ps->m_pNext->m_pPrev = ps->m_pPrev;
		return Delete(ps);
	}
	template<typename T, typename Traits>
	void TinyLinkList< T, Traits>::RemoveAll()
	{
		while (m_dwCount > 0)
		{
			TinyEntry* ps = m_pFirst;
			ASSERT(ps);
			m_pFirst = ps->m_pNext;
			Delete(ps);
		}
		m_pFirst = NULL;
		m_pLast = NULL;
		m_pEntry = NULL;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
	}
	template<typename T, typename Traits>
	DWORD TinyLinkList< T, Traits>::GetSize() const
	{
		return m_dwCount;
	}
	template<typename T, typename Traits>
	BOOL TinyLinkList< T, Traits>::IsEmpty() const
	{
		return m_dwCount == 0;
	}
	template<typename T, typename Traits>
	ITERATOR TinyLinkList< T, Traits>::operator[](const T& value) const
	{
		return Lookup(value, NULL);
	}
	/// <summary>
	/// 红黑树Map 
	/// </summary>
	template<class K, class V, class KTraits = DefaultTraits< K >, class VTraits = DefaultTraits< V >>
	class TinyMap
	{
		DISALLOW_COPY_AND_ASSIGN(TinyMap);
	private:
		class TinyEntry : public __ITERATOR
		{
		public:
			K			m_key;
			V			m_value;
			BOOL		m_bColor;
			TinyEntry*	m_pLeft;
			TinyEntry*	m_pRight;
			TinyEntry*	m_pParent;
		public:
			TinyEntry(const K& key, const V& value)
				:m_key(key),
				m_value(value),
				m_bColor(FALSE),
				m_pLeft(NULL),
				m_pRight(NULL),
				m_pParent(NULL)
			{

			}
		};
	public:
		explicit TinyMap(DWORD dwBlockSize = 10);
		~TinyMap();
		ITERATOR operator[](const K& key) const;
		DWORD GetSize() const;
		BOOL Contain(const K& key) const;
		BOOL IsEmpty() const;
		ITERATOR Add(const K& key, const V& value);
		BOOL Remove(const K& key);
		void RemoveAll();
		ITERATOR Lookup(const K& key);
		const ITERATOR Lookup(const K& key) const;
		ITERATOR SetAt(const K& key, V& value);
		V* GetValueAt(ITERATOR pos);
		const K* GetKeyAt(ITERATOR pos) const;
		const V* GetValueAt(ITERATOR pos) const;
		V* GetValue(const K& key);
		const V* GetValue(const K& key) const;
		ITERATOR First() const;
		ITERATOR Last() const;
		ITERATOR Prev(ITERATOR pos) const;
		ITERATOR Next(ITERATOR pos) const;
	private:
		typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* New(const K& key, const V& value);
		typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* Lookup(TinyEntry* ps, const K& key);
		void RotateL(TinyEntry* ps) throw();
		void RotateR(TinyEntry* ps) throw();
		void Add(TinyEntry* ps) throw();
		void AddFixup(TinyEntry* ps) throw();
		void Delete(TinyEntry* ps) throw();
		void DeletePostOrder(TinyEntry* ps) throw();
		void Remove(TinyEntry* ps) throw();
		void RemoveFixup(TinyEntry* ps) throw();
		void SwapNode(TinyEntry* ps1, TinyEntry* ps2);
		TinyEntry*	Minimum(TinyEntry* ps)const throw();
		TinyEntry*	Maximum(TinyEntry* ps)const throw();
		TinyEntry*	Prev(TinyEntry* ps) const throw();
		TinyEntry*	Next(TinyEntry* ps) const throw();
		BOOL IsNil(TinyEntry *p) const throw();
		void SetNil(TinyEntry **p) throw();
	private:
		DWORD		m_dwBlockSize;
		DWORD		m_dwCount;
		TinyEntry*	m_pRoot;
		TinyEntry*	m_pFree;
		TinyEntry*	m_pNIL;//哨兵所有的叶子节点指向这个节点
		TinyPlex*	m_pBlocks;
	};
	template<class K, class V, class KTraits, class VTraits>
	TinyMap<K, V, KTraits, VTraits>::TinyMap(DWORD dwBlockSize)
		:m_dwBlockSize(dwBlockSize),
		m_dwCount(0),
		m_pBlocks(NULL),
		m_pFree(NULL),
		m_pNIL(NULL),
		m_pRoot(NULL)
	{

	}
	template<class K, class V, class KTraits, class VTraits>
	TinyMap<K, V, KTraits, VTraits>::~TinyMap()
	{
		RemoveAll();
		SAFE_FREE(m_pNIL);
	}
	template<class K, class V, class KTraits, class VTraits>
	DWORD TinyMap<K, V, KTraits, VTraits>::GetSize() const
	{
		return m_dwCount;
	}
	template<class K, class V, class KTraits, class VTraits>
	BOOL TinyMap<K, V, KTraits, VTraits>::IsEmpty() const
	{
		return m_dwCount == 0;
	}
	template<class K, class V, class KTraits, class VTraits>
	BOOL TinyMap<K, V, KTraits, VTraits>::Contain(const K& key) const
	{
		return Lookup(m_pRoot, key) != NULL;
	}
	template<class K, class V, class KTraits, class VTraits>
	BOOL TinyMap<K, V, KTraits, VTraits>::IsNil(TinyEntry *p) const throw()
	{
		return (p == m_pNIL);
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::SetNil(TinyEntry **p) throw()
	{
		*p = m_pNIL;
	}

	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::New(const K& key, const V& value)
	{
		if (m_pFree == NULL)
		{
			//初始化哨兵节点
			if (m_pNIL == NULL)
			{
				m_pNIL = reinterpret_cast<TinyEntry*>(malloc(sizeof(TinyEntry)));
				memset(m_pNIL, 0x00, sizeof(TinyEntry));
				m_pNIL->m_bColor = FALSE;
				m_pNIL->m_pParent = m_pNIL->m_pLeft = m_pNIL->m_pRight = m_pNIL;
				m_pRoot = m_pNIL;
			}
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_dwBlockSize, sizeof(TinyEntry));
			if (pPlex == NULL) return NULL;
			TinyEntry* ps = static_cast<TinyEntry*>(pPlex->data());
			ps += m_dwBlockSize - 1;
			for (INT_PTR iBlock = m_dwBlockSize - 1; iBlock >= 0; iBlock--)
			{
				ps->m_pLeft = m_pFree;
				m_pFree = ps;
				ps--;
			}
		}
		TinyEntry* pNew = m_pFree;
		::new(pNew)TinyEntry(key, value);
		m_pFree = m_pFree->m_pLeft;
		pNew->m_bColor = TRUE;
		//新节点指向哨兵节点
		SetNil(&pNew->m_pLeft);
		SetNil(&pNew->m_pRight);
		SetNil(&pNew->m_pParent);
		m_dwCount++;
		return pNew;
	}
	template<class K, class V, class KTraits, class VTraits>
	BOOL TinyMap<K, V, KTraits, VTraits>::Remove(const K& key)
	{
		TinyEntry* ps = Lookup(m_pRoot, key);
		if (ps != NULL)
		{
			Remove(ps);
			return TRUE;
		}
		return FALSE;
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::RemoveAll()
	{
		DeletePostOrder(m_pRoot);
		m_dwCount = 0;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
		m_pFree = NULL;
		m_pRoot = NULL;
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::Add(const K& key, const V& value)
	{
		TinyEntry* ps = Lookup(m_pRoot, key);
		if (ps == NULL)
		{
			TinyEntry* pNew = New(key, value);
			if (pNew != NULL)
			{
				Add(pNew);
				return ITERATOR(pNew);
			}
		}
		return NULL;
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::Add(TinyEntry* pNew) throw()
	{
		TinyEntry* pX = m_pRoot;
		TinyEntry* pY = NULL;
		while (!IsNil(pX))
		{
			pY = pX;
			if (KTraits::Compare(pNew->m_key, pX->m_key) <= 0)
			{
				pX = pX->m_pLeft;
			}
			else
			{
				pX = pX->m_pRight;
			}
		}
		pNew->m_pParent = pY;
		if (pY == NULL)
		{
			m_pRoot = pNew;
		}
		else if (KTraits::Compare(pNew->m_key, pY->m_key) <= 0)
		{
			pY->m_pLeft = pNew;
		}
		else
		{
			pY->m_pRight = pNew;
		}
		AddFixup(pNew);
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::AddFixup(TinyEntry* pNew) throw()
	{
		TinyEntry* pX = pNew;
		pX->m_bColor = TRUE;
		TinyEntry* pY = NULL;
		while (pX != m_pRoot && pX->m_pParent->m_bColor)//当前节点不是根节点，并且节点的父节点颜色为红色
		{
			//父节点为祖父节点的左孩子
			if (pX->m_pParent == pX->m_pParent->m_pParent->m_pLeft)//Left
			{
				pY = pX->m_pParent->m_pParent->m_pRight;//取叔叔节点
				//情况1:父节点为红，叔叔节点为红
				if (pY != NULL && pY->m_bColor)//red
				{
					pY->m_bColor = FALSE;//叔叔节点改为黑色
					pX->m_pParent->m_bColor = FALSE;//父节点修改为黑色
					pX->m_pParent->m_pParent->m_bColor = TRUE;//祖父节点修改为红色
					pX = pX->m_pParent->m_pParent;
				}
				else//情况2:父节点为红，叔叔节点为黑色，当前节点为父节点的右孩子
				{
					if (pX == pX->m_pParent->m_pRight)
					{
						pX = pX->m_pParent;//修改当前节点为父节点
						RotateL(pX);//左旋当前节点
					}
					//情况3:父节点为红，叔叔节点为黑色，当前节点为父节点的左孩子
					pX->m_pParent->m_bColor = FALSE;//父节点修改为黑色
					pX->m_pParent->m_pParent->m_bColor = TRUE;//祖父节点修改为红色
					RotateR(pX->m_pParent->m_pParent);//右旋祖父节点
				}
			}
			else//父节点为祖父节点的右孩子，这也有三种情况，和上述情况操作类似，只不过旋转方向相反
			{
				//情况1
				pY = pX->m_pParent->m_pParent->m_pLeft;
				if (pY != NULL && pY->m_bColor)//red
				{
					pX->m_pParent->m_bColor = FALSE;
					pY->m_bColor = FALSE;
					pX->m_pParent->m_pParent->m_bColor = TRUE;
					pX = pX->m_pParent->m_pParent;
				}
				else
				{
					//情况2
					if (pX == pX->m_pParent->m_pLeft)
					{
						pX = pX->m_pParent;
						RotateR(pX);
					}
					//情况3
					pX->m_pParent->m_bColor = FALSE;
					pX->m_pParent->m_pParent->m_bColor = TRUE;
					RotateL(pX->m_pParent->m_pParent);
				}
			}
		}
		//根节点永远是黑色
		m_pRoot->m_bColor = FALSE;
		SetNil(&m_pRoot->m_pParent);
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::Delete(TinyEntry* ps)
	{
		ASSERT(ps);
		ps->~TinyEntry();
		ps->m_pLeft = m_pFree;
		m_pFree = ps;
		m_dwCount--;
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::DeletePostOrder(TinyEntry* ps) throw()
	{
		if (IsNil(ps) || ps == NULL) return;
		DeletePostOrder(ps->m_pLeft);
		DeletePostOrder(ps->m_pRight);
		Delete(ps);
	}
	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::Minimum(TinyEntry* ps) const throw()
	{
		if (IsNil(ps) || ps == NULL)
			return (NULL);
		TinyEntry* pMin = ps;
		while (!IsNil(pMin->m_pLeft))
		{
			pMin = pMin->m_pLeft;
		}
		return pMin;
	}
	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::Maximum(TinyEntry* ps) const throw()
	{
		if (IsNil(ps) || ps == NULL)
			return (NULL);
		TinyEntry* pMax = ps;
		while (!IsNil(pMax->m_pLeft))
		{
			pMax = pMax->m_pLeft;
		}
		return pMax;
	}
	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::Prev(TinyEntry* ps) const throw()
	{
		if (ps == NULL)
		{
			return(NULL);
		}
		if (!IsNil(ps->m_pLeft))
		{
			return(Maximum(ps->m_pLeft));
		}

		TinyEntry* pParent = ps->m_pParent;
		TinyEntry* pLeft = ps;
		while (!IsNil(pParent) && (pLeft == pParent->m_pLeft))
		{
			pLeft = pParent;
			pParent = pParent->m_pParent;
		}

		if (IsNil(pParent))
		{
			pParent = NULL;
		}
		return(pParent);
	}
	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::Next(TinyEntry* ps) const throw()
	{
		if (ps == NULL)
		{
			return (NULL);
		}
		if (!IsNil(ps->m_pRight))
		{
			return Minimum(ps->m_pRight);
		}
		TinyEntry* pParent = ps->m_pParent;
		TinyEntry* pRight = ps;
		while (!IsNil(pParent) && (pRight == pParent->m_pRight))
		{
			pRight = pParent;
			pParent = pParent->m_pParent;
		}
		if (IsNil(pParent))
		{
			pParent = NULL;
		}
		return pParent;
	}

	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::Remove(TinyEntry* pZ)
	{
		if (pZ == NULL) return;
		TinyEntry* pY = NULL;
		TinyEntry* pX = NULL;
		if (IsNil(pZ->m_pLeft) || IsNil(pZ->m_pRight))//左子节点为哨兵
		{
			pY = pZ;//记录后继节点，用着个节点代替删除节点
		}
		else
		{
			pY = Next(pZ);//获得后继节点
		}
		if (!IsNil(pY->m_pLeft))
		{
			pX = pY->m_pLeft;
		}
		else
		{
			pX = pY->m_pRight;
		}

		pX->m_pParent = pY->m_pParent;

		if (IsNil(pY->m_pParent))
		{
			m_pRoot = pX;
		}
		else if (pY == pY->m_pParent->m_pLeft)
		{
			pY->m_pParent->m_pLeft = pX;
		}
		else
		{
			pY->m_pParent->m_pRight = pX;
		}
		//如果后继是红色，就不需要调整红黑树
		if (!pY->m_bColor)
		{
			RemoveFixup(pX);
		}
		if (pY != pZ)
		{
			SwapNode(pY, pZ);
		}

		if (m_pRoot != NULL)
		{
			SetNil(&m_pRoot->m_pParent);
		}
		Delete(pZ);
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::RemoveFixup(TinyEntry* ps)
	{
		TinyEntry* pX = ps;
		TinyEntry* pW = NULL;

		while ((pX != m_pRoot) && (!pX->m_bColor))
		{
			//当前节点为左节点的情况
			if (pX == pX->m_pParent->m_pLeft)
			{
				pW = pX->m_pParent->m_pRight;
				//情况1:当前节点为黑色，兄弟节点为红色
				if (pW->m_bColor)
				{
					pW->m_bColor = FALSE;//兄弟节点修改为黑色
					pX->m_pParent->m_bColor = TRUE;//父节点修改红色
					RotateL(pX->m_pParent);
					pW = pX->m_pParent->m_pRight;
				}
				//情况2:当前节点为黑色，兄弟节点为黑色，兄弟节点的俩子节点也为黑色
				if (!pW->m_pLeft->m_bColor && !pW->m_pRight->m_bColor)
				{
					pW->m_bColor = TRUE;//兄弟节点修改为红色
					pX = pX->m_pParent;//当前节点指向父节点
				}
				else
				{
					//情况3:当前节点为黑色，兄弟节点为黑色，兄弟节点右子节点为黑色，左子节点为红色
					if (!pW->m_pRight->m_bColor)
					{
						pW->m_pLeft->m_bColor = FALSE;//左子节点改为黑色
						pW->m_bColor = TRUE;//兄弟节点改为红色
						RotateR(pW);
						pW = pX->m_pParent->m_pRight;
					}
					//情况4:当前节点为黑色，兄弟节点为黑色，兄弟节点右子节点为红色，左子节点任意色
					pW->m_bColor = pX->m_pParent->m_bColor;
					pX->m_pParent->m_bColor = FALSE;
					pW->m_pRight->m_bColor = FALSE;
					RotateL(pX->m_pParent);
					pX = m_pRoot;
				}
			}
			else//当前节点为右节点的情况
			{
				pW = pX->m_pParent->m_pLeft;
				if (pW->m_bColor)
				{
					pW->m_bColor = FALSE;
					pX->m_pParent->m_bColor = TRUE;
					RotateR(pX->m_pParent);
					pW = pX->m_pParent->m_pLeft;
				}
				if (!pW->m_pRight->m_bColor && !pW->m_pLeft->m_bColor)
				{
					pW->m_bColor = TRUE;
					pX = pX->m_pParent;
				}
				else
				{
					if (!pW->m_pLeft->m_bColor)
					{
						pW->m_pRight->m_bColor = FALSE;
						pW->m_bColor = TRUE;
						RotateL(pW);
						pW = pX->m_pParent->m_pLeft;
					}
					pW->m_bColor = pX->m_pParent->m_bColor;
					pX->m_pParent->m_bColor = FALSE;
					pW->m_pLeft->m_bColor = FALSE;
					RotateR(pX->m_pParent);
					pX = m_pRoot;
				}
			}
		}
		pX->m_bColor = FALSE;
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::SwapNode(TinyEntry* pDest, TinyEntry* pSrc)
	{
		pDest->m_pParent = pSrc->m_pParent;
		if (pSrc->m_pParent->m_pLeft == pSrc)
		{
			pSrc->m_pParent->m_pLeft = pDest;
		}
		else
		{
			pSrc->m_pParent->m_pRight = pDest;
		}

		pDest->m_pRight = pSrc->m_pRight;
		pDest->m_pLeft = pSrc->m_pLeft;
		pDest->m_bColor = pSrc->m_bColor;
		pDest->m_pRight->m_pParent = pDest;
		pDest->m_pLeft->m_pParent = pDest;

		if (m_pRoot == pSrc)
		{
			m_pRoot = pDest;
		}
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::RotateL(TinyEntry* ps) throw()
	{
		if (ps == NULL) return;
		TinyEntry* pTemp = ps->m_pRight;
		ps->m_pRight = pTemp->m_pLeft;
		if (!IsNil(pTemp->m_pLeft))
		{
			pTemp->m_pLeft->m_pParent = ps;
		}
		pTemp->m_pParent = ps->m_pParent;
		if (IsNil(ps->m_pParent))
		{
			m_pRoot = pTemp;
		}
		else if (ps == ps->m_pParent->m_pLeft)
		{
			ps->m_pParent->m_pLeft = pTemp;
		}
		else
		{
			ps->m_pParent->m_pRight = pTemp;
		}
		pTemp->m_pLeft = ps;
		ps->m_pParent = pTemp;
	}
	template<class K, class V, class KTraits, class VTraits>
	void TinyMap<K, V, KTraits, VTraits>::RotateR(TinyEntry* ps) throw()
	{
		if (ps == NULL) return;
		TinyEntry* pTemp = ps->m_pLeft;
		ps->m_pLeft = pTemp->m_pRight;
		if (!IsNil(pTemp->m_pRight))
		{
			pTemp->m_pRight->m_pParent = ps;
		}
		pTemp->m_pParent = ps->m_pParent;
		if (IsNil(ps->m_pParent))
		{
			m_pRoot = pTemp;
		}
		else if (ps == ps->m_pParent->m_pRight)
		{
			ps->m_pParent->m_pRight = pTemp;
		}
		else
		{
			ps->m_pParent->m_pLeft = pTemp;
		}
		pTemp->m_pRight = ps;
		ps->m_pParent = pTemp;
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::Lookup(const K& key)
	{
		if (!m_pRoot) return NULL;
		return Lookup(m_pRoot, key);
	}
	template<class K, class V, class KTraits, class VTraits>
	const ITERATOR TinyMap<K, V, KTraits, VTraits>::Lookup(const K& key) const
	{
		if (!m_pRoot) return NULL;
		return Lookup(m_pRoot, key);
	}
	template<class K, class V, class KTraits, class VTraits>
	typename TinyMap<K, V, KTraits, VTraits>::TinyEntry* TinyMap<K, V, KTraits, VTraits>::Lookup(TinyEntry* ps, const K& key)
	{
		TinyEntry* pKey = NULL;
		TinyEntry* pX = ps;
		while (!IsNil(pX) && (pKey == NULL))
		{
			INT cmp = KTraits::Compare(key, pX->m_key);
			if (cmp == 0)
			{
				pKey = pX;
			}
			else
			{
				if (cmp < 0)
				{
					pX = pX->m_pLeft;
				}
				else
				{
					pX = pX->m_pRight;
				}
			}
		}
		if (pKey == NULL)
		{
			return(NULL);
		}
#pragma warning(push)
#pragma warning(disable:4127)
		for (;;)
		{
			TinyEntry* pPrev = Prev(pKey);
			if ((pPrev != NULL) && KTraits::Compare(key, pPrev->m_key))
			{
				pKey = pPrev;
			}
			else
			{
				return(pKey);
			}
		}
#pragma warning(pop)
	}
	template<class K, class V, class KTraits, class VTraits>
	V* TinyMap<K, V, KTraits, VTraits>::GetValueAt(ITERATOR pos)
	{
		if (!pos) return NULL;
		TinyEntry* ps = static_cast <TinyEntry*>(pos);
		if (!ps) return NULL;
		return &ps->m_value;
	}
	template<class K, class V, class KTraits, class VTraits>
	V* TinyMap<K, V, KTraits, VTraits>::GetValue(const K& key)
	{
		ITERATOR pos = Lookup(key);
		if (!pos) return NULL;
		return GetValueAt(pos);
	}
	template<class K, class V, class KTraits, class VTraits>
	const K* TinyMap<K, V, KTraits, VTraits>::GetKeyAt(ITERATOR pos) const
	{
		if (!pos) return NULL;
		TinyEntry* ps = static_cast <TinyEntry*>(pos);
		if (!ps) return NULL;
		return &ps->m_key;
	}
	template<class K, class V, class KTraits, class VTraits>
	const V* TinyMap<K, V, KTraits, VTraits>::GetValueAt(ITERATOR pos) const
	{
		if (!pos) return NULL;
		TinyEntry* ps = static_cast <TinyEntry*>(pos);
		if (!ps) return NULL;
		return &ps->m_value;
	}
	template<class K, class V, class KTraits, class VTraits>
	const V* TinyMap<K, V, KTraits, VTraits>::GetValue(const K& key) const
	{
		ITERATOR pos = Lookup(key);
		if (!pos) return NULL;
		return GetValueAt(pos);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::First() const
	{
		if (!m_pRoot) return NULL;
		return Prev(m_pRoot);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::Last() const
	{
		if (!m_pRoot) return NULL;
		return Next(m_pRoot);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::Next(ITERATOR pos) const
	{
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		return Next(ps);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::Prev(ITERATOR pos) const
	{
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		return Prev(ps);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::operator[](const K& key) const
	{
		return Lookup(key);
	}
	template<class K, class V, class KTraits, class VTraits>
	ITERATOR TinyMap<K, V, KTraits, VTraits>::SetAt(const K& key, V& value)
	{
		ITERATOR pos = Lookup(key);
		TinyEntry* ps = static_cast<TinyEntry*>(pos);
		if (ps != NULL)
		{
			ps->m_value = value;
		}
		else
		{
			pos = Add(key, value);
		}
		return pos;
	}
	/// <summary>
	/// HashMap结构
	/// </summary>
	template<class K, class V, class KTraits = DefaultTraits< K >, class VTraits = DefaultTraits< V >>
	class TinyHashMap
	{

	};
}
