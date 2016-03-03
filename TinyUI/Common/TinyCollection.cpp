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
	TinyMapPtr::TinyMapPtr(DWORD dwBlockSize)
		:m_dwBlockSize(dwBlockSize),
		m_dwHashSize(17),
		m_dwCount(0),
		m_pHashTable(NULL),
		m_pFreeList(NULL),
		m_pBlocks(NULL)
	{

	}
	DWORD TinyMapPtr::GetSize() const
	{
		return m_dwCount;
	}
	BOOL TinyMapPtr::IsEmpty() const
	{
		return m_dwCount == 0;
	}
	void* TinyMapPtr::Lookup(void* key) const
	{
		return NULL;
	}
	void* TinyMapPtr::operator[](void* key) const
	{
		return NULL;
	}

	void TinyMapPtr::Add(void* key, void* value)
	{

	}
	void TinyMapPtr::Remove(void* key)
	{

	}
	void TinyMapPtr::RemoveAll()
	{

	}
}
