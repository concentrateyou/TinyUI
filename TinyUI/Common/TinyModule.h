#pragma once
#include "TinyObject.h"
#include <Psapi.h>
#include <TlHelp32.h>

namespace TinyUI
{
	class TinyEnumModule
	{
	public:
		virtual	BOOL	Initialize() = 0;
		virtual HMODULE	operator[](INT index) = 0;
		virtual DWORD	GetSize() = 0;
		virtual INT		Lookup(HMODULE m_hModule) = 0;
		virtual BOOL	GetModule(INT index, MODULEINFO& info) = 0;
		virtual BOOL	GetModule(HMODULE module, MODULEINFO& info) = 0;
	};
	/// <summary>
	/// Psapi获得进程模块信息
	/// </summary>
	class TinyPsapiEnumModule : public TinyEnumModule
	{
		typedef BOOL(WINAPI* PFNENUMPROCESSMODULES) (HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
		typedef BOOL(WINAPI* PFNGETMODULEINFORMATION) (HANDLE hProcess, HMODULE hModule, MODULEINFO* lpmodinfo, DWORD cb);
	public:
		TinyPsapiEnumModule(DWORD dwProcessId, HMODULE hModPSAPI);
		~TinyPsapiEnumModule();
		BOOL		Initialize();
		HMODULE		operator[](INT index);
		INT			Lookup(HMODULE m_hModule);
		DWORD		GetSize();
		BOOL		GetModule(INT index, MODULEINFO& info);
		BOOL		GetModule(HMODULE module, MODULEINFO& info);
	private:
		PFNENUMPROCESSMODULES	m_pfnEnumProcessModules;
		PFNGETMODULEINFORMATION m_pfnGetModuleInformation;
		HMODULE					m_hModPSAPI;
		DWORD					m_dwProcessId;
		HMODULE*				m_pModules;
		DWORD					m_dwSize;
	};
	/// <summary>
	/// ToolHelp获得进程模块信息
	/// </summary>
	class TinyToolHelpEnumModule : public TinyEnumModule
	{
		typedef BOOL(WINAPI* PFNMODULE32FIRST) (HANDLE hSnapshot, LPMODULEENTRY32 lpme);
		typedef BOOL(WINAPI* PFNMODULE32NEXT) (HANDLE hSnapshot, LPMODULEENTRY32 lpme);
		typedef HANDLE(WINAPI* PFNCREATETOOLHELP32SNAPSHOT) (DWORD dwFlags, DWORD th32ProcessID);
	public:
		TinyToolHelpEnumModule(DWORD dwProcessId);
		~TinyToolHelpEnumModule();
		BOOL		Initialize();
		HMODULE		operator[](INT index);
		INT			Lookup(HMODULE m_hModule);
		DWORD		GetSize();
		BOOL		GetModule(INT index, MODULEINFO& info);
		BOOL		GetModule(HMODULE module, MODULEINFO& info);
	private:
		HANDLE									m_hSnapshot;
		PFNCREATETOOLHELP32SNAPSHOT				m_pfnCreateToolhelp32Snapshot;
		PFNMODULE32NEXT							m_pfnModule32Next;
		PFNMODULE32FIRST						m_pfnModule32First;
		DWORD									m_dwProcessId;
		//TinyArray<MODULEENTRY32>				m_modules;
	};
};

