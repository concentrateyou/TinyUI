#include "../stdafx.h"
#include "TinyModule.h"

namespace TinyFramework
{
	TinyPsapiEnumModule::TinyPsapiEnumModule(DWORD dwProcessId, HMODULE hModPSAPI)
		:m_hModPSAPI(hModPSAPI),
		m_dwProcessId(dwProcessId),
		m_pModules(NULL),
		m_dwSize(0),
		m_pfnEnumProcessModules(NULL),
		m_pfnGetModuleInformation(NULL)
	{
		if (m_hModPSAPI)
		{
			m_pfnEnumProcessModules = (PFNENUMPROCESSMODULES) ::GetProcAddress(m_hModPSAPI, "EnumProcessModules");
			m_pfnGetModuleInformation = (PFNGETMODULEINFORMATION) ::GetProcAddress(m_hModPSAPI, "GetModuleInformation");
		}
	}

	TinyPsapiEnumModule::~TinyPsapiEnumModule()
	{
		SAFE_DELETE_ARRAY(m_pModules);
	}

	BOOL TinyPsapiEnumModule::Initialize()
	{
		if (!m_pfnEnumProcessModules || !m_pfnGetModuleInformation) return FALSE;
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_dwProcessId);
		if (!hProcess) return FALSE;
		DWORD cb = 0;
		HMODULE hModule = NULL;
		if (!m_pfnEnumProcessModules(hProcess, &hModule, sizeof(hModule), &cb) || !cb)
		{
			::CloseHandle(hProcess);
			return FALSE;
		}
		m_dwSize = cb / sizeof(HMODULE);
		m_pModules = new HMODULE[m_dwSize];
		if (!m_pfnEnumProcessModules(hProcess, m_pModules, cb, &cb))
		{
			::CloseHandle(hProcess);
			return FALSE;
		}
		::CloseHandle(hProcess);
		return TRUE;
	}
	DWORD TinyPsapiEnumModule::GetSize()
	{
		return m_dwSize;
	}
	INT	TinyPsapiEnumModule::Lookup(HMODULE hModule)
	{
		ASSERT(hModule);
		for (DWORD i = 0; i < m_dwSize; i++)
		{
			if (m_pModules[i] == hModule)
			{
				return i;
			}
		}
		return -1;
	}
	HMODULE TinyPsapiEnumModule::operator[](INT index)
	{
		if (index < 0 || index >= (INT)m_dwSize || !m_pModules)
			return NULL;
		return m_pModules[index];
	}

	BOOL TinyPsapiEnumModule::GetModule(INT index, MODULEINFO& modeinfo)
	{
		if (index < 0 || index >= (INT)m_dwSize || !m_pModules)
			return FALSE;
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_dwProcessId);
		if (!hProcess)
			return FALSE;
		if (!m_pfnGetModuleInformation(hProcess, m_pModules[index], &modeinfo, sizeof(MODULEINFO)))
		{
			::CloseHandle(hProcess);
			return FALSE;
		}
		::CloseHandle(hProcess);
		return TRUE;
	}

	BOOL TinyPsapiEnumModule::GetModule(HMODULE module, MODULEINFO& modeinfo)
	{
		if (!module || !m_pModules)
			return FALSE;
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_dwProcessId);
		if (!hProcess)
			return FALSE;
		if (!m_pfnGetModuleInformation(hProcess, module, &modeinfo, sizeof(MODULEINFO)))
		{
			::CloseHandle(hProcess);
			return FALSE;
		}
		::CloseHandle(hProcess);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyToolHelpEnumModule::TinyToolHelpEnumModule(DWORD dwProcessId)
		:m_hSnapshot(NULL),
		m_dwProcessId(dwProcessId),
		m_pfnCreateToolhelp32Snapshot(NULL),
		m_pfnModule32Next(NULL),
		m_pfnModule32First(NULL)
	{

	}

	TinyToolHelpEnumModule::~TinyToolHelpEnumModule()
	{
		if (m_hSnapshot != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hSnapshot);
			m_hSnapshot = NULL;
		}
	}

	BOOL TinyToolHelpEnumModule::Initialize()
	{
		HMODULE hInstance = GetModuleHandle(_T("KERNEL32"));
		if (!hInstance)	return FALSE;
		m_pfnCreateToolhelp32Snapshot = (PFNCREATETOOLHELP32SNAPSHOT)::GetProcAddress(hInstance, "CreateToolhelp32Snapshot");
#ifdef UNICODE
		m_pfnModule32First = (PFNMODULE32FIRST)::GetProcAddress(hInstance, "Module32FirstW");
		m_pfnModule32Next = (PFNMODULE32NEXT)::GetProcAddress(hInstance, "Module32NextW");
#else
		m_pfnModule32First = (PFNMODULE32FIRST)::GetProcAddress(hInstance, "Module32First");
		m_pfnModule32Next = (PFNMODULE32NEXT)::GetProcAddress(hInstance, "Module32Next");
#endif
		m_hSnapshot = INVALID_HANDLE_VALUE;
		if (m_pfnCreateToolhelp32Snapshot == NULL ||
			m_pfnModule32First == NULL ||
			m_pfnModule32Next == NULL)
		{
			return FALSE;
		}
		MODULEENTRY32 me;
		ZeroMemory(&me, sizeof(MODULEENTRY32));
		me.dwSize = sizeof(MODULEENTRY32);
		HANDLE hSnapshot = m_pfnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
		if (hSnapshot == INVALID_HANDLE_VALUE) return FALSE;
		if (!m_pfnModule32First(hSnapshot, &me)) return FALSE;
		do
		{
			m_modules.Add(me);
		} while (m_pfnModule32Next(hSnapshot, &me));
		CloseHandle(hSnapshot);
		return TRUE;
	}
	HMODULE	TinyToolHelpEnumModule::operator[](INT index)
	{
		if (index < 0 || index >= m_modules.GetSize())
			return NULL;
		MODULEENTRY32& me = m_modules[index];
		return me.hModule;
	}
	INT	TinyToolHelpEnumModule::Lookup(HMODULE m_hModule)
	{
		for (INT i = 0; i < m_modules.GetSize(); i++)
		{
			MODULEENTRY32& me = m_modules[i];
			if (me.hModule == m_hModule)
			{
				return i;
			}
		}
		return -1;
	}
	DWORD TinyToolHelpEnumModule::GetSize()
	{
		return m_modules.GetSize();
	}
	BOOL TinyToolHelpEnumModule::GetModule(INT index, MODULEINFO& info)
	{
		if (index < 0 || index >= m_modules.GetSize())
			return FALSE;
		MODULEENTRY32& me = m_modules[index];
		info.lpBaseOfDll = me.modBaseAddr;
		info.EntryPoint = 0;
		info.SizeOfImage = me.modBaseSize;
		return TRUE;
	}
	BOOL TinyToolHelpEnumModule::GetModule(HMODULE hModule, MODULEINFO& info)
	{
		if (!hModule)
			return FALSE;
		for (INT i = 0; i < m_modules.GetSize(); i++)
		{
			MODULEENTRY32& me = m_modules[i];
			if (me.hModule == hModule)
			{
				info.lpBaseOfDll = me.modBaseAddr;
				info.EntryPoint = 0;
				info.SizeOfImage = me.modBaseSize;
				break;
			}
		};
		return TRUE;
	}
};
