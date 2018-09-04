#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinyMemoryPool.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyMemoryAlloc::TinyMemoryAlloc()
			:m_pNodeFree(NULL),
			m_pBlocks(NULL),
			m_size(0),
			m_count(0)
		{
		}

		TinyMemoryAlloc::~TinyMemoryAlloc()
		{
			FreeAll();
		}
		UINT32 TinyMemoryAlloc::GetElementSize()
		{
			return m_size;
		}
		void TinyMemoryAlloc::Initialize(UINT32 count, UINT32 size)
		{
			ASSERT(size >= sizeof(TinyNode));
			if (size < sizeof(TinyNode))
				size = sizeof(TinyNode);
			if (count <= 1)
				count = 3;
			m_size = size;
			m_count = count;
			m_pNodeFree = NULL;
			m_pBlocks = NULL;
		}
		void TinyMemoryAlloc::FreeAll()
		{
			if (m_pBlocks != NULL)
			{
				m_pBlocks->Destory();
				m_pBlocks = NULL;
			}
			m_pNodeFree = NULL;
			m_count = 0;
			m_size = 0;
		}

		void* TinyMemoryAlloc::Alloc()
		{
			if (m_pNodeFree == NULL)
			{
				TinyPlex* pNewBlock = TinyPlex::Create(m_pBlocks, m_count, m_size);
				TinyNode* pNode = (TinyNode*)pNewBlock->data();
				(BYTE*&)pNode += (m_size * m_count) - m_size;
				for (UINT32 i = m_count - 1; i >= 0; i--, (BYTE*&)pNode -= m_size)
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

		void TinyMemoryAlloc::Free(void* ps)
		{
			if (ps != NULL)
			{
				TinyNode* pNode = (TinyNode*)ps;
				pNode->pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyMemoryPool::TinyMemoryPool()
		{
			InitializeCriticalSection(&m_cs);
		}

		TinyMemoryPool::~TinyMemoryPool()
		{
			DeleteCriticalSection(&m_cs);
		}

		void TinyMemoryPool::FreeAll()
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

		void* TinyMemoryPool::Alloc()
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

		void TinyMemoryPool::Free(void* p)
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
