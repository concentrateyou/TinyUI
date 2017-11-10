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
	TinyPointerMap::TinyPointerMap()
		:m_blockSize(10),
		m_size(17),
		m_count(0),
		m_ppTable(NULL),
		m_pFreeList(NULL),
		m_pBlocks(NULL)
	{

	}
	TinyPointerMap::~TinyPointerMap()
	{
		RemoveAll();
	}
	DWORD TinyPointerMap::GetSize() const
	{
		return m_count;
	}
	BOOL TinyPointerMap::IsEmpty() const
	{
		return m_count == 0;
	}
	void TinyPointerMap::Initialize(UINT size)
	{
		SAFE_DELETE_ARRAY(m_ppTable);
		m_ppTable = new TinyNode*[size];
		memset(m_ppTable, 0, sizeof(TinyNode*) * size);
		m_size = size;
	}
	TinyPointerMap::TinyNode* TinyPointerMap::New(UINT_PTR key, UINT_PTR value)
	{
		if (m_pFreeList == NULL)
		{
			TinyPlex* pPlex = TinyPlex::Create(m_pBlocks, m_blockSize, sizeof(TinyNode));
			if (pPlex == NULL) return NULL;
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
		::new(pNew)TinyNode(key, value);
		pNew->m_key = key;
		pNew->m_value = value;
		m_pFreeList = m_pFreeList->m_pNext;
		m_count++;
		return pNew;
	}
	void TinyPointerMap::Delete(TinyNode* ps)
	{
		ps->m_pNext = m_pFreeList;
		m_pFreeList = ps;
		m_count--;
	}
	BOOL TinyPointerMap::Add(UINT_PTR key, UINT_PTR value)
	{
		UINT index = 0;
		UINT_PTR hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, index, hash)) == NULL)
		{
			if (!m_ppTable)
			{
				Initialize(m_size);
			}
			ps = New();
			ps->m_key = key;
			ps->m_value = value;
			ps->m_pNext = m_ppTable[index];
			m_ppTable[index] = ps;
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyPointerMap::Remove(UINT_PTR key)
	{
		if (!m_ppTable)
			return FALSE;
		TinyNode** ppPrev = NULL;
		UINT index = TinyHashKey<UINT_PTR>(key) % m_size;
		ppPrev = &m_ppTable[index];
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
		SAFE_DELETE_ARRAY(m_ppTable);
		m_count = 0;
		m_pFreeList = NULL;
		m_pBlocks->Destory();
		m_pBlocks = NULL;
	}
	BOOL TinyPointerMap::Lookup(UINT_PTR key, UINT_PTR& value) const
	{
		UINT index = 0;
		UINT_PTR hash = 0;
		TinyNode* ps = Lookup(key, index, hash);
		if (ps == NULL)
			return FALSE;
		value = ps->m_value;
		return TRUE;
	}
	TinyPointerMap::TinyNode* TinyPointerMap::Lookup(UINT_PTR key, UINT& index, UINT_PTR& hash) const
	{
		hash = TinyHashKey<UINT_PTR>(key);
		index = hash % m_size;
		if (!m_ppTable) return NULL;
		TinyNode* ps = NULL;
		for (ps = m_ppTable[index]; ps != NULL; ps = ps->m_pNext)
		{
			if (ps->m_key == key)
			{
				return ps;
			}
		}
		return NULL;
	}
	UINT_PTR& TinyPointerMap::operator[](UINT_PTR key)
	{
		UINT index = 0;
		UINT_PTR hash = 0;
		TinyNode* ps = NULL;
		if ((ps = Lookup(key, index, hash)) == NULL)
		{
			if (!m_ppTable)
			{
				Initialize(m_size);
			}
			ps = New();
			ps->m_key = key;
			ps->m_pNext = m_ppTable[index];
			m_ppTable[index] = ps;
		}
		return ps->m_value;
	}
	void TinyPointerMap::SetAt(UINT_PTR key, UINT_PTR value)
	{
		(*this)[key] = value;
	}
}
