#include "../stdafx.h"
#include "TinyCollection.h"
#include <exception>

namespace TinyFramework
{
	TinyPlex* PASCAL TinyPlex::Create(TinyPlex*& ps, UINT_PTR nMax, UINT_PTR nElementSize)
	{
		ASSERT(nMax > 0 && nElementSize > 0);
		TinyPlex* p = NULL;
		p = (TinyPlex*)new BYTE[sizeof(TinyPlex) + nMax * nElementSize];
		if (p == NULL)
			return NULL;
		p->pNext = ps;
		ps = p;
		return p;
	}
	void TinyPlex::Destory()
	{
		TinyPlex* ps = NULL;
		ps = this;
		while (ps != NULL)
		{
			BYTE* bits = (BYTE*)ps;
			TinyPlex* pNext = ps->pNext;
			SAFE_DELETE_ARRAY(bits);
			ps = pNext;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinySimpleList::TinySimpleList(INT offset)
		:m_pFirst(NULL),
		m_offset(offset)
	{

	}
	void TinySimpleList::Construct(INT offset)
	{
		ASSERT(m_pFirst == NULL);
		m_offset = offset;
	}
	BOOL TinySimpleList::IsEmpty() const
	{
		return m_pFirst == NULL;
	}
	void** TinySimpleList::GetNextPtr(void* ps) const
	{
		ASSERT(ps != NULL);
		return (void**)((BYTE*)ps + m_offset);
	}
	void TinySimpleList::RemoveAll()
	{
		m_pFirst = NULL;
	}
	void* TinySimpleList::GetFirst() const
	{
		return m_pFirst;
	}
	void* TinySimpleList::GetNext(void* ps) const
	{
		return *GetNextPtr(ps);
	}
	void TinySimpleList::Add(void* ps)
	{
		ASSERT(ps != NULL);
		ASSERT(*GetNextPtr(ps) == NULL);
		*GetNextPtr(ps) = m_pFirst;
		m_pFirst = ps;
	}

	BOOL TinySimpleList::Remove(void* ps)
	{
		ASSERT(ps != NULL);
		if (m_pFirst == NULL)
			return FALSE;
		BOOL bRes = FALSE;
		if (m_pFirst == ps)
		{
			m_pFirst = *GetNextPtr(ps);
			bRes = TRUE;
		}
		else
		{
			void* pTemp = m_pFirst;
			while (pTemp != NULL && *GetNextPtr(pTemp) != ps)
				pTemp = *GetNextPtr(pTemp);
			if (pTemp != NULL)
			{
				*GetNextPtr(pTemp) = *GetNextPtr(ps);
				bRes = TRUE;
			}
		}
		return bRes;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyPointerMap::TinyPointerMap(INT_PTR blockSize)
		:m_blockSize(blockSize),
		m_hashSize(17),
		m_count(0),
		m_pTable(NULL),
		m_pFreeList(NULL),
		m_pBlocks(NULL)
	{
		m_blockSize = m_blockSize == 0 ? 10 : m_blockSize;
	}
	TinyPointerMap::~TinyPointerMap()
	{
		RemoveAll();
	}
	INT_PTR TinyPointerMap::GetSize() const
	{
		return m_count;
	}
	BOOL TinyPointerMap::IsEmpty() const
	{
		return (m_count == 0);
	}
	void TinyPointerMap::Initialize(UINT hashSize)
	{
		if (hashSize == 0)
			hashSize = 17;
		SAFE_DELETE_ARRAY(m_pTable);
		m_pTable = new TinyNode*[hashSize];
		memset(m_pTable, 0, sizeof(TinyNode*) * hashSize);
		m_hashSize = hashSize;
	}
	TinyPointerMap::TinyNode* TinyPointerMap::New()
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
		pNew->m_key = NULL;
		pNew->m_value = NULL;
		return pNew;
	}
	void TinyPointerMap::Delete(TinyNode* ps)
	{
		ps->m_pNext = m_pFreeList;
		m_pFreeList = ps;
		m_count--;
	}
	BOOL TinyPointerMap::Add(LPVOID key, LPVOID value)
	{
		UINT index = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, index, hash)) == NULL)
		{
			if (m_pTable == NULL)
			{
				Initialize(m_hashSize);
			}
			ps = New();
			ps->m_key = key;
			ps->m_value = value;
			ps->m_pNext = m_pTable[index];
			m_pTable[index] = ps;
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyPointerMap::Remove(LPVOID key)
	{
		if (!m_pTable)
			return FALSE;
		TinyNode** ppPrev = NULL;
		UINT bucket = HashKey(key) % m_hashSize;
		ppPrev = &m_pTable[bucket];
		TinyNode* ps = NULL;
		for (ps = *ppPrev; ps != NULL; ps = ps->m_pNext)
		{
			if (ps->m_key == key)
			{
				*ppPrev = ps->m_pNext;
				Delete(ps);
				return TRUE;
			}
			ppPrev = &ps->m_pNext;
		}
		return FALSE;
	}
	void TinyPointerMap::RemoveAll()
	{
		SAFE_DELETE_ARRAY(m_pTable);
		m_count = 0;
		m_pFreeList = NULL;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
	}
	BOOL TinyPointerMap::Lookup(LPVOID key, LPVOID& value) const
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
	TinyPointerMap::TinyNode* TinyPointerMap::Lookup(LPVOID key, UINT& bucket, UINT& hash) const
	{
		hash = HashKey(key);
		bucket = hash % m_hashSize;
		if (m_pTable == NULL)
			return NULL;
		TinyNode* ps = NULL;
		for (ps = m_pTable[bucket]; ps != NULL; ps = ps->m_pNext)
		{
			if (ps->m_key == key)
			{
				return ps;
			}
		}
		return NULL;
	}
	LPVOID& TinyPointerMap::operator[](LPVOID key)
	{
		UINT bucket = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, bucket, hash)) == NULL)
		{
			if (!m_pTable)
			{
				Initialize(m_hashSize);
			}
			ps = New();
			ps->m_key = key;
			ps->m_pNext = m_pTable[bucket];
			m_pTable[bucket] = ps;
		}
		return ps->m_value;
	}
	void TinyPointerMap::SetAt(LPVOID key, LPVOID value)
	{
		(*this)[key] = value;
	}
	UINT TinyPointerMap::HashKey(void* key) const
	{
		return (TinyHashKey<DWORD_PTR>((DWORD_PTR)key));
	}
}
