#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include <string>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		class TinyFixedAllocNoSync
		{
		public:
			TinyFixedAllocNoSync(UINT nAllocSize, UINT nBlockSize = 64);
			~TinyFixedAllocNoSync();
			UINT GetAllocSize();
		public:
			void* Alloc();
			void Free(void* p);
			void FreeAll();
		protected:
			struct TinyNode
			{
				TinyNode* pNext;
			};
			UINT		m_nAllocSize;
			UINT		m_nBlockSize;
			TinyPlex*	m_pBlocks;
			TinyNode*	m_pNodeFree;
		};
		/// <summary>
		/// 线程安全的固定大小内存池
		/// </summary>
		class TinyFixedAlloc : public TinyFixedAllocNoSync
		{
			typedef class TinyFixedAllocNoSync base;
		public:
			TinyFixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);
			~TinyFixedAlloc();
		public:
			void* Alloc();
			void Free(void* p);
			void FreeAll();
		protected:
			CRITICAL_SECTION m_cs;
		};
	}
}

