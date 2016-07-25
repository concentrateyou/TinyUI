#include "stdafx.h"
#include "SharedMemory.h"

namespace D3D
{
	typedef BOOL(WINAPI *SystemFunction036_T)(PVOID, ULONG);
	SystemFunction036_T SystemFunction036;

	string _cdecl Format(const CHAR* s, ...)
	{
		va_list args;
		va_start(args, s);
		INT size;
		CHAR szBuffer[512];
		size = _vsntprintf_s(szBuffer, 512, s, args);
		ASSERT(size >= 0);
		va_end(args);
		return string(szBuffer, 0, size);
	}
	void RandBytes(void* output, size_t outputSize)
	{
		CScopedLibrary advapi32(TEXT("advapi32.dll"));
		SystemFunction036 = (SystemFunction036_T)GetProcAddress(advapi32, "SystemFunction036");
		char* pOutput = static_cast<char*>(output);
		while (outputSize > 0)
		{
			const ULONG outputBytes = static_cast<ULONG>(std::min<size_t>(outputSize, static_cast<size_t>((std::numeric_limits<ULONG>::max)())));
			const BOOL success = SystemFunction036(pOutput, outputBytes) != FALSE;
			outputSize -= outputBytes;
			pOutput += outputBytes;
		}
	}
	ULONGLONG RandUInteger64()
	{
		ULONGLONG number;
		RandBytes(&number, sizeof(number));
		return number;
	}
	ULONGLONG RandGenerator(ULONGLONG range)
	{
		ULONGLONG max_acceptable_value = ((std::numeric_limits<ULONGLONG>::max)() / range) * range - 1;
		ULONGLONG value;
		do
		{
			value = RandUInteger64();
		} while (value > max_acceptable_value);

		return value % range;
	}
	INT RandInteger(INT min, INT max)
	{
		ULONGLONG range = static_cast<LONGLONG>(max)-min + 1;
		return static_cast<INT>(min + static_cast<LONGLONG>(RandGenerator(range)));
	}

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
	CSharedMemory::CSharedMemory()
		:m_hFileMap(NULL),
		m_bReadonly(FALSE),
		m_pMemory(NULL)
	{

	}
	CSharedMemory::~CSharedMemory()
	{
		Close();
	}
	CSharedMemory::operator HANDLE() const
	{
		return m_hFileMap;
	}
	BOOL CSharedMemory::Create(const string& name, DWORD dwSize)
	{
		m_name = name;
		SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, FALSE };
		SECURITY_DESCRIPTOR sd;
		ACL dacl = { 0 };
		sa.lpSecurityDescriptor = &sd;
		if (!InitializeAcl(&dacl, sizeof(dacl), ACL_REVISION))
		{
			return FALSE;
		}
		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		{
			return FALSE;
		}
		if (!SetSecurityDescriptorDacl(&sd, TRUE, &dacl, FALSE))
		{
			return FALSE;
		}
		if (m_name.empty())
		{
			ULONGLONG values[4];
			RandBytes(&values, sizeof(values));
			m_name = Format("SharedMemory__%016llx%016llx%016llx%016llx", values[0], values[1], values[2], values[3]);
		}
		m_hFileMap = CreateFileMappingReducedPermissions(&sa, dwSize, m_name.empty() ? NULL : m_name.c_str());
		return m_hFileMap != NULL;
	}
	BOOL CSharedMemory::Delete()
	{
		return TRUE;
	}
	BOOL CSharedMemory::Open(const string& name, BOOL bReadonly)
	{
		DWORD dwAccess = FILE_MAP_READ | SECTION_QUERY;
		if (!bReadonly) dwAccess |= FILE_MAP_WRITE;
		m_bReadonly = bReadonly;
		m_hFileMap = OpenFileMapping(dwAccess, false, name.empty() ? NULL : name.c_str());
		if (!m_hFileMap)
			return FALSE;
		return TRUE;
	}
	BOOL CSharedMemory::Close()
	{
		if (!m_hFileMap)
			return FALSE;
		return ::CloseHandle(m_hFileMap);
	}
	BOOL CSharedMemory::Map(ULONGLONG offset, DWORD dwBytes)
	{
		if (!m_hFileMap || m_pMemory)
			return FALSE;
		m_pMemory = MapViewOfFile(m_hFileMap, m_bReadonly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE, static_cast<ULONGLONG>(offset) >> 32, static_cast<DWORD>(offset), dwBytes);
		if (!m_pMemory)
			return FALSE;
		m_dwMapSize = GetMemorySectionSize(m_pMemory);
		return TRUE;
	}
	BOOL CSharedMemory::Unmap()
	{
		if (m_pMemory &&
			UnmapViewOfFile(m_pMemory))
		{
			m_pMemory = NULL;
			return TRUE;
		}
		return FALSE;
	}
	void* CSharedMemory::Address() const
	{
		return m_pMemory;
	}
	BOOL CSharedMemory::IsValid() const
	{
		return m_hFileMap != NULL;
	}
}
