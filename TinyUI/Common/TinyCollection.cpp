#include "../stdafx.h"
#include "TinyCollection.h"
#include <exception>

namespace TinyUI
{
	TinyPlex* PASCAL TinyPlex::Create(TinyPlex*& ps, UINT_PTR nMax, UINT_PTR nElementSize)
	{
		ASSERT(nMax > 0 && nElementSize > 0);
		TinyPlex* p = (TinyPlex*)new BYTE[sizeof(TinyPlex) + nMax * nElementSize];
		if (p == NULL) return NULL;
		p->pNext = ps;
		ps = p;
		return p;
	}
	void TinyPlex::Destory()
	{
		TinyPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*)p;
			TinyPlex* pNext = p->pNext;
			delete[] bytes;
			p = pNext;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyPtrMap::TinyPtrMap()
		:m_dwBlockSize(10),
		m_dwHashSize(17),
		m_dwCount(0),
		m_ppHashTable(NULL),
		m_pFreeList(NULL),
		m_pBlocks(NULL)
	{

	}
	TinyPtrMap::~TinyPtrMap()
	{
		RemoveAll();
	}
	DWORD	TinyPtrMap::GetSize() const
	{
		return m_dwCount;
	}
	BOOL	TinyPtrMap::IsEmpty() const
	{
		return m_dwCount == 0;
	}
	void TinyPtrMap::Initialize(DWORD dwHashSize)
	{
		SAFE_DELETE_ARRAY(m_ppHashTable);
		m_ppHashTable = new TinyNode*[dwHashSize];
		memset(m_ppHashTable, 0, sizeof(TinyNode*) * dwHashSize);
		m_dwHashSize = dwHashSize;
	}
	TinyPtrMap::TinyNode* TinyPtrMap::New(UINT_PTR key, UINT_PTR value)
	{
		if (m_pFreeList == NULL)
		{
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_dwBlockSize, sizeof(TinyNode));
			if (pPlex == NULL) return NULL;
			TinyNode* ps = static_cast<TinyNode*>(pPlex->data());
			ps += m_dwBlockSize - 1;
			for (INT_PTR iBlock = m_dwBlockSize - 1; iBlock >= 0; iBlock--)
			{
				ps->m_pNext = m_pFreeList;
				m_pFreeList = ps;
				ps--;
			}
		}
		TinyNode* pNew = m_pFreeList;
		::new(pNew)TinyNode(key, value);
		pNew->m_key = key;
		pNew->m_value = value;
		m_pFreeList = m_pFreeList->m_pNext;
		m_dwCount++;
		return pNew;
	}
	void TinyPtrMap::Delete(TinyNode* ps)
	{
		ps->m_pNext = m_pFreeList;
		m_pFreeList = ps;
		m_dwCount--;
	}
	BOOL TinyPtrMap::Add(UINT_PTR key, UINT_PTR value)
	{
		UINT index = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, index, hash)) == NULL)
		{
			if (!m_ppHashTable)
			{
				Initialize(m_dwHashSize);
			}
			ps = New();
			ps->m_key = key;
			ps->m_value = value;
			ps->m_pNext = m_ppHashTable[index];
			m_ppHashTable[index] = ps;
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyPtrMap::Remove(UINT_PTR key)
	{
		if (!m_ppHashTable)
			return FALSE;
		TinyNode** ppPrev = NULL;
		UINT index = TinyHashKey<UINT_PTR>(key) % m_dwHashSize;
		ppPrev = &m_ppHashTable[index];
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
	void TinyPtrMap::RemoveAll()
	{
		SAFE_DELETE_ARRAY(m_ppHashTable);
		m_dwCount = 0;
		m_pFreeList = NULL;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
	}
	BOOL TinyPtrMap::Lookup(UINT_PTR key, UINT_PTR& value) const
	{
		UINT index = 0;
		UINT hash = 0;
		TinyNode* ps = Lookup(key, index, hash);
		if (ps == NULL)
			return FALSE;
		value = ps->m_value;
		return TRUE;
	}
	TinyPtrMap::TinyNode* TinyPtrMap::Lookup(UINT_PTR key, UINT& index, UINT_PTR& hash) const
	{
		hash = TinyHashKey<UINT_PTR>(key);
		index = hash % m_dwHashSize;
		if (!m_ppHashTable) return NULL;
		TinyNode* ps = NULL;
		for (ps = m_ppHashTable[index]; ps != NULL; ps = ps->m_pNext)
		{
			if (ps->m_key == key)
			{
				return ps;
			}
		}
		return NULL;
	}
	UINT_PTR& TinyPtrMap::operator[](UINT_PTR key)
	{
		UINT index = 0;
		UINT hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, index, hash)) == NULL)
		{
			if (!m_ppHashTable)
			{
				Initialize(m_dwHashSize);
			}
			ps = New();
			ps->m_key = key;
			ps->m_pNext = m_ppHashTable[index];
			m_ppHashTable[index] = ps;
		}
		return ps->m_value;
	}
	void TinyPtrMap::SetAt(UINT_PTR key, UINT_PTR value)
	{
		(*this)[key] = value;
	}
}
