#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinySharedMemory.h"

namespace TinyUI
{
	namespace IO
	{
		typedef BOOL(WINAPI *SystemFunction036_T)(PVOID, ULONG);
		SystemFunction036_T SystemFunction036;

		HANDLE CreateFileMappingReducedPermissions(SECURITY_ATTRIBUTES* sa, DWORD dwSize, LPCSTR name)
		{
			HANDLE hFile = CreateFileMapping(INVALID_HANDLE_VALUE, sa, PAGE_READWRITE, 0, dwSize, name);
			if (!hFile)
			{
				return NULL;
			}
			HANDLE hHandle = NULL;
			if (::DuplicateHandle(GetCurrentProcess(), hFile, GetCurrentProcess(), &hHandle, FILE_MAP_READ | FILE_MAP_WRITE | SECTION_QUERY, FALSE, 0))
			{
				::CloseHandle(hFile);
				return hHandle;
			}
			::CloseHandle(hFile);
			return NULL;
		}
		DWORD GetMemorySectionSize(void* address)
		{
			MEMORY_BASIC_INFORMATION info;
			if (!::VirtualQuery(address, &info, sizeof(info)))
				return 0;
			return info.RegionSize - (static_cast<char*>(address)-static_cast<char*>(info.AllocationBase));
		}
		TinySharedMemory::TinySharedMemory()
			:m_hFileMap(NULL),
			m_bReadonly(FALSE),
			m_pMemory(NULL)
		{

		}
		TinySharedMemory::~TinySharedMemory()
		{
			Close();
		}
		TinySharedMemory::operator HANDLE() const
		{
			return m_hFileMap;
		}
		BOOL TinySharedMemory::Create(const TinyString& name, DWORD dwSize)
		{
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwSize, name.STR());
			return m_hFileMap != NULL;
		}
		BOOL TinySharedMemory::Delete()
		{
			return TRUE;
		}
		BOOL TinySharedMemory::Open(const TinyString& name, BOOL bReadonly)
		{
			DWORD dwAccess = FILE_MAP_READ | SECTION_QUERY;
			if (!bReadonly) dwAccess |= FILE_MAP_WRITE;
			m_bReadonly = bReadonly;
			m_hFileMap = OpenFileMapping(dwAccess, false, name.IsEmpty() ? NULL : name.STR());
			if (!m_hFileMap)
				return FALSE;
			return TRUE;
		}
		BOOL TinySharedMemory::Close()
		{
			if (!m_hFileMap)
				return FALSE;
			return ::CloseHandle(m_hFileMap);
		}
		BOOL TinySharedMemory::Map(ULONGLONG offset, DWORD dwBytes)
		{
			if (!m_hFileMap || m_pMemory)
				return FALSE;
			m_pMemory = MapViewOfFile(m_hFileMap, m_bReadonly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE, static_cast<ULONGLONG>(offset) >> 32, static_cast<DWORD>(offset), dwBytes);
			if (!m_pMemory)
				return FALSE;
			m_dwMapSize = GetMemorySectionSize(m_pMemory);
			return TRUE;
		}
		BOOL TinySharedMemory::Unmap()
		{
			if (m_pMemory &&
				UnmapViewOfFile(m_pMemory))
			{
				m_pMemory = NULL;
				return TRUE;
			}
			return FALSE;
		}
		void* TinySharedMemory::Address() const
		{
			return m_pMemory;
		}
		BOOL TinySharedMemory::IsValid() const
		{
			return m_hFileMap != NULL;
		}
	}
}
