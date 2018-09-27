#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinySharedMemory.h"
#include "../Common/TinyLogging.h"

namespace TinyFramework
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
			return info.RegionSize - (static_cast<char*>(address) - static_cast<char*>(info.AllocationBase));
		}
		typedef enum _SECTION_INFORMATION_CLASS
		{
			SectionBasicInformation,
		} SECTION_INFORMATION_CLASS;
		typedef ULONG(__stdcall* NtQuerySectionType)(
			HANDLE SectionHandle,
			SECTION_INFORMATION_CLASS SectionInformationClass,
			PVOID SectionInformation,
			ULONG SectionInformationLength,
			PULONG ResultLength);
		typedef struct _SECTION_BASIC_INFORMATION
		{
			PVOID BaseAddress;
			ULONG Attributes;
			LARGE_INTEGER Size;
		} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;
		BOOL IsSectionSafeToMap(HANDLE handle)
		{
			static NtQuerySectionType nqst;
			if (!nqst)
			{
				nqst = reinterpret_cast<NtQuerySectionType>(::GetProcAddress(::GetModuleHandle("ntdll.dll"), "NtQuerySection"));
				if (!nqst)
					return FALSE;
			}
			SECTION_BASIC_INFORMATION sbi = {};
			ULONG status = nqst(handle, SectionBasicInformation, &sbi, sizeof(sbi), nullptr);
			if (status)
				return FALSE;
			return (sbi.Attributes & SEC_IMAGE) != SEC_IMAGE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinySharedMemory::TinySharedMemory()
			:m_hFileMap(NULL),
			m_bReadonly(FALSE),
			m_address(NULL)
		{

		}
		TinySharedMemory::~TinySharedMemory()
		{
			Unmap();
			Close();
		}
		TinySharedMemory::operator HANDLE() const
		{
			return m_hFileMap;
		}
		BOOL TinySharedMemory::Create(const TinyString& name, DWORD dwSize)
		{
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwSize, name.STR());
			m_dwSize = dwSize;
			return m_hFileMap != NULL;
		}
		BOOL TinySharedMemory::Delete()
		{
			return TRUE;
		}
		BOOL TinySharedMemory::Open(const TinyString& name, BOOL bReadonly)
		{
			DWORD dwAccess = FILE_MAP_READ | SECTION_QUERY;
			if (!bReadonly)
				dwAccess |= FILE_MAP_WRITE;
			m_bReadonly = bReadonly;
			m_hFileMap = OpenFileMapping(dwAccess, FALSE, name.IsEmpty() ? NULL : name.STR());
			if (!m_hFileMap)
				return FALSE;
			return TRUE;
		}
		void TinySharedMemory::Close()
		{
			m_dwSize = 0;
			m_dwMapSize = 0;
			Unmap();
			SAFE_CLOSE_HANDLE(m_hFileMap);
		}
		BOOL TinySharedMemory::Map(ULONGLONG offset, DWORD dwBytes)
		{
			if (!m_hFileMap)
				return FALSE;
			Unmap();
			m_address = MapViewOfFile(m_hFileMap, m_bReadonly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS, static_cast<ULONGLONG>(offset) >> 32, static_cast<DWORD>(offset), dwBytes);
			if (!m_address)
			{
				LOG(ERROR) << "MapViewOfFile Error: " << GetLastError();
				return FALSE;
			}
			m_dwMapSize = dwBytes;
			return TRUE;
		}
		void TinySharedMemory::Unmap()
		{
			if (m_address != NULL)
			{
				UnmapViewOfFile(m_address);
			}
			m_address = NULL;
		}
		void* TinySharedMemory::Address() const
		{
			return m_address;
		}
		BOOL TinySharedMemory::IsEmpty() const
		{
			return m_hFileMap == NULL;
		}
		DWORD TinySharedMemory::GetMapSize() const
		{
			return m_dwMapSize;
		}
		DWORD TinySharedMemory::GetSize() const
		{
			return m_dwSize;
		}
	}
}
