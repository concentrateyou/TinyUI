#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include <string>
using namespace std;

#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)

namespace TinyFramework
{
	namespace IO
	{
		class TinyMemoryAlloc
		{
		public:
			TinyMemoryAlloc();
			~TinyMemoryAlloc();
			UINT32	GetElementSize();
			void	Initialize(UINT32 count, UINT32 size = 64);
		public:
			void*	Alloc();
			void	Free(void* p);
			void	FreeAll();
		protected:
			struct TinyNode
			{
				TinyNode* pNext;
			};
		protected:
			UINT32		m_size;
			UINT32		m_count;
			TinyPlex*	m_pBlocks;
			TinyNode*	m_pNodeFree;
		};
		/// <summary>
		/// 线程安全的固定大小内存池
		/// </summary>
		class TinyMemoryPool : public TinyMemoryAlloc
		{
			typedef class TinyMemoryAlloc base;
		public:
			TinyMemoryPool();
			~TinyMemoryPool();
		public:
			void*	Alloc();
			void	Free(void* p);
			void	FreeAll();
		protected:
			CRITICAL_SECTION m_cs;
		};
	}
}

