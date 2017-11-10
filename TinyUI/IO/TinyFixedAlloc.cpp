#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinyFixedAlloc.h"

namespace TinyUI
{
	namespace IO
	{
		TinyFixedAllocNoSync::TinyFixedAllocNoSync()
			:m_pNodeFree(NULL),
			m_pBlocks(NULL),
			m_allocSize(0),
			m_blockSize(0)
		{
		}

		TinyFixedAllocNoSync::~TinyFixedAllocNoSync()
		{
			FreeAll();
		}
		UINT TinyFixedAllocNoSync::GetAllocSize()
		{
			return m_allocSize;
		}
		void TinyFixedAllocNoSync::Initialize(UINT blockSize, UINT allocSize)
		{
			ASSERT(allocSize >= sizeof(TinyNode));

			if (allocSize < sizeof(TinyNode))
				allocSize = sizeof(TinyNode);
			if (blockSize <= 1)
				blockSize = 3;

			m_allocSize = allocSize;
			m_blockSize = blockSize;
			m_pNodeFree = NULL;
			m_pBlocks = NULL;
		}
		void TinyFixedAllocNoSync::FreeAll()
		{
			m_pBlocks->Destory();
			m_pBlocks = NULL;
			m_pNodeFree = NULL;
			m_blockSize = 0;
			m_allocSize = 0;
		}

		void* TinyFixedAllocNoSync::Alloc()
		{
			if (m_pNodeFree == NULL)
			{
				TinyPlex* pNewBlock = TinyPlex::Create(m_pBlocks, m_blockSize, m_allocSize);
				TinyNode* pNode = (TinyNode*)pNewBlock->data();
				(BYTE*&)pNode += (m_allocSize * m_blockSize) - m_allocSize;
				for (INT i = m_blockSize - 1; i >= 0; i--, (BYTE*&)pNode -= m_allocSize)
				{
					pNode->pNext = m_pNodeFree;
					m_pNodeFree = pNode;
				}
			}
			ASSERT(m_pNodeFree != NULL);
			void* pNode = m_pNodeFree;
			m_pNodeFree = m_pNodeFree->pNext;
			return pNode;
		}

		void TinyFixedAllocNoSync::Free(void* p)
		{
			if (p != NULL)
			{
				TinyNode* pNode = (TinyNode*)p;
				pNode->pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyFixedAlloc::TinyFixedAlloc()
		{
			InitializeCriticalSection(&m_cs);
		}

		TinyFixedAlloc::~TinyFixedAlloc()
		{
			DeleteCriticalSection(&m_cs);
		}

		void TinyFixedAlloc::FreeAll()
		{
			EnterCriticalSection(&m_cs);
			__try
			{
				base::FreeAll();
			}
			__finally
			{
				LeaveCriticalSection(&m_cs);
			}
		}

		void* TinyFixedAlloc::Alloc()
		{
			EnterCriticalSection(&m_cs);
			void* ps = NULL;
			try
			{
				ps = base::Alloc();
			}
			catch (...)
			{
				LeaveCriticalSection(&m_cs);
				throw;
			}
			LeaveCriticalSection(&m_cs);
			return ps;
		}

		void TinyFixedAlloc::Free(void* p)
		{
			if (p != NULL)
			{
				EnterCriticalSection(&m_cs);
				__try
				{
					base::Free(p);
				}
				__finally
				{
					LeaveCriticalSection(&m_cs);
				}
			}
		}
	}
}
