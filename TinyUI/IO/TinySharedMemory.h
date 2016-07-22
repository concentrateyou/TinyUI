#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinySemaphore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include <string>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 共享内存
		/// </summary>
		class TinySharedMemory
		{
			DISALLOW_COPY_AND_ASSIGN(TinySharedMemory)
		public:
			TinySharedMemory();
			~TinySharedMemory();
			operator HANDLE() const;
			BOOL	Create(const TinyString& name, DWORD dwSize);
			BOOL	Delete();
			BOOL	Open(const TinyString& name, BOOL bReadonly = FALSE);
			BOOL	Close();
			BOOL	Map(ULONGLONG offset = 0, DWORD dwBytes = 0);
			BOOL	Unmap();
			void*	Address() const;
			BOOL	IsValid() const;
		private:
			TinyString	m_name;
			BOOL        m_bReadonly;
			HANDLE		m_hFileMap;
			void*       m_pMemory;
			DWORD		m_dwMapSize;
		};
	};
}

