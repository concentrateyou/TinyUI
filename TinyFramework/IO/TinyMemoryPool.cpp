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
		BOOL TinyMemoryAlloc::IsEmpty() const
		{
			return m_pBlocks == NULL;
		}
		UINT32 TinyMemoryAlloc::GetElementSize() const
		{
			return m_size;
		}
		void TinyMemoryAlloc::Initialize(UINT32 count, UINT32 size)
		{
			ASSERT(size >= sizeof(TinyNode));
			m_size = size;
			m_count = count <= 1 ? 3 : count;
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
				for (INT32 i = m_count - 1; i >= 0; i--, (BYTE*&)pNode -= m_size)
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

		void TinyMemoryPool::Initialize(UINT32 count, UINT32 size)
		{
			base::Initialize(count, size + sizeof(TinyNode));
		}

		void* TinyMemoryPool::Alloc()
		{
			EnterCriticalSection(&m_cs);
			BYTE* ps = NULL;
			try
			{
				ps = static_cast<BYTE*>(base::Alloc());
				ps += sizeof(TinyNode);
			}
			catch (...)
			{
				LeaveCriticalSection(&m_cs);
				throw;
			}
			LeaveCriticalSection(&m_cs);
			return static_cast<void*>(ps);
		}

		void TinyMemoryPool::Free(void* p)
		{
			if (p != NULL)
			{
				EnterCriticalSection(&m_cs);
				__try
				{
					BYTE* ps = static_cast<BYTE*>(p);
					ps -= sizeof(TinyNode);
					base::Free(static_cast<void*>(ps));
				}
				__finally
				{
					LeaveCriticalSection(&m_cs);
				}
			}
		}
	}
}
