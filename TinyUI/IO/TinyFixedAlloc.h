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
		class TinyFixedAlloc
		{
			DISALLOW_COPY_AND_ASSIGN(TinyFixedAlloc)
		public:
			TinyFixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);
			~TinyFixedAlloc();
		public:
			void* Alloc();
			void Free(void* p);
			void FreeAll();
		protected:
			struct TinyNode
			{
				TinyNode* pNext;
			};
		protected:
			UINT				m_nAllocSize;
			UINT				m_nBlockSize;
			TinyPlex*			m_pBlocks;
			TinyNode*			m_pNodeFree;
			CRITICAL_SECTION	m_cs;
		};

#define DECLARE_FIXED_ALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		ASSERT(size == m_alloc.GetAllocSize()); \
		UNUSED(size); \
		return m_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { m_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		ASSERT(size == m_alloc.GetAllocSize()); \
		UNUSED(size); \
		return m_alloc.Alloc(); \
	} \
protected: \
	static TinyFixedAlloc m_alloc \
	};

#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) \
TinyFixedAlloc class_name::m_alloc(sizeof(class_name), block_size) 

	}
}

