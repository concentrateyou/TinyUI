#pragma once
#include "D3DCommon.h"
#include <string>
#include <Ntsecapi.h>
#include <algorithm>
#include <limits>
using namespace std;

namespace D3D
{
	class CSharedMemory
	{
		DISALLOW_COPY_AND_ASSIGN(CSharedMemory)
	public:
		CSharedMemory();
		~CSharedMemory();
		operator HANDLE() const;
		BOOL	Create(const string& name, DWORD dwSize);
		BOOL	Delete();
		BOOL	Open(const string& name, BOOL bReadonly = FALSE);
		BOOL	Close();
		BOOL	Map(ULONGLONG offset = 0, DWORD dwBytes = 0);
		BOOL	Unmap();
		void*	Address() const;
		BOOL	IsValid() const;
	private:
		BOOL        m_bReadonly;
		HANDLE		m_hFileMap;
		void*       m_pMemory;
		DWORD		m_dwMapSize;
	};
}

