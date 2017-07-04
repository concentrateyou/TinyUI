#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinyFixedAlloc.h"

namespace TinyUI
{
	namespace IO
	{
		TinyFixedAlloc::TinyFixedAlloc(UINT nAllocSize, UINT nBlockSize)
		{
			ASSERT(nAllocSize >= sizeof(TinyNode));
			ASSERT(nBlockSize > 1);
			if (nAllocSize < sizeof(TinyNode))
				nAllocSize = sizeof(TinyNode);
			if (nBlockSize <= 1)
				nBlockSize = 64;
			m_nAllocSize = nAllocSize;
			m_nBlockSize = nBlockSize;
			m_pNodeFree = NULL;
			m_pBlocks = NULL;
			InitializeCriticalSection(&m_cs);
		}

		TinyFixedAlloc::~TinyFixedAlloc()
		{
			FreeAll();
			DeleteCriticalSection(&m_cs);
		}

		void TinyFixedAlloc::FreeAll()
		{
			EnterCriticalSection(&m_cs);
			__try
			{
				m_pBlocks->Destory();
				m_pBlocks = NULL;
				m_pNodeFree = NULL;
			}
			__finally
			{
				LeaveCriticalSection(&m_cs);
			}
		}

		void* TinyFixedAlloc::Alloc()
		{
			EnterCriticalSection(&m_cs);
			try
			{
				if (m_pNodeFree == NULL)
				{
					TinyPlex* pNewBlock = TinyPlex::Create(m_pBlocks, m_nBlockSize, m_nAllocSize);
					TinyNode* pNode = (TinyNode*)pNewBlock->data();
					(BYTE*&)pNode += (m_nAllocSize * m_nBlockSize) - m_nAllocSize;
					for (INT i = m_nBlockSize - 1; i >= 0; i--, (BYTE*&)pNode -= m_nAllocSize)
					{
						pNode->pNext = m_pNodeFree;
						m_pNodeFree = pNode;
					}
				}
				ASSERT(m_pNodeFree != NULL);
				void* pNode = m_pNodeFree;
				m_pNodeFree = m_pNodeFree->pNext;
				LeaveCriticalSection(&m_cs);
				return pNode;
			}
			catch (...)
			{
				LeaveCriticalSection(&m_cs);
				throw;
			}
			LeaveCriticalSection(&m_cs);
			return NULL;
		}

		void TinyFixedAlloc::Free(void* p)
		{
			if (p != NULL)
			{
				EnterCriticalSection(&m_cs);
				__try
				{
					TinyNode* pNode = (TinyNode*)p;
					pNode->pNext = m_pNodeFree;
					m_pNodeFree = pNode;
				}
				__finally
				{
					LeaveCriticalSection(&m_cs);
				}
			}
			
		}
	}
}
