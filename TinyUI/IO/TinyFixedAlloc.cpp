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
			m_cbElement(0),
			m_count(0)
		{
		}

		TinyFixedAllocNoSync::~TinyFixedAllocNoSync()
		{
			FreeAll();
		}
		UINT TinyFixedAllocNoSync::GetAllocSize()
		{
			return m_cbElement;
		}
		void TinyFixedAllocNoSync::Initialize(UINT nBlockSize, UINT nAllocSize)
		{
			ASSERT(nAllocSize >= sizeof(TinyNode));
			ASSERT(nBlockSize > 1);

			if (nAllocSize < sizeof(TinyNode))
				nAllocSize = sizeof(TinyNode);
			if (nBlockSize <= 1)
				nBlockSize = 64;

			m_cbElement = nAllocSize;
			m_count = nBlockSize;
			m_pNodeFree = NULL;
			m_pBlocks = NULL;
		}
		void TinyFixedAllocNoSync::FreeAll()
		{
			m_pBlocks->Destory();
			m_pBlocks = NULL;
			m_pNodeFree = NULL;
		}

		void* TinyFixedAllocNoSync::Alloc()
		{
			if (m_pNodeFree == NULL)
			{
				TRACE("TinyFixedAllocNoSync New Alloc\n");
				TinyPlex* pNewBlock = TinyPlex::Create(m_pBlocks, m_count, m_cbElement);
				TinyNode* pNode = (TinyNode*)pNewBlock->data();
				(BYTE*&)pNode += (m_cbElement * m_count) - m_cbElement;
				for (INT i = m_count - 1; i >= 0; i--, (BYTE*&)pNode -= m_cbElement)
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
