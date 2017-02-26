#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include "TinyString.h"

#if defined(_M_X64) || defined(__x86_64__)
#include "../HDE/hde64.h"
typedef hde64s HDE;
#define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#else
#include "../HDE/hde32.h"
typedef hde32s HDE;
#define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#endif

namespace TinyUI
{
	class TinyIATFunction;
	class TinyIATHook;
	/// <summary>
	/// IAT×¢Èë
	/// </summary>
	class TinyIATHook : public TinyReference < TinyIATHook >
	{
	protected:
		typedef struct tagEXCLUDEDMODULE
		{
			HMODULE		hModule;
			TCHAR		pzModule[MAX_PATH];
		}EXCLUDEDMODULE;
	public:
		TinyIATHook();
		virtual ~TinyIATHook();
	public:
		BOOL Add(LPCSTR pszCalleeModName, LPCSTR pszFunctionName, PROC  pfnOrig, PROC  pfnHook);
		BOOL Remove(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
		void RemoveAll();
		BOOL IsModuleExclude(HMODULE hModule);
		void ExcludeModule(LPCTSTR lpszModule);
		TinyArray<HMODULE> GetExcludeModules();
		TinyIATFunction* GetFunction(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	public:
		static PROC GetOriginalProc(TinyIATHook* pHook, LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	protected:
		TinyArray<TinyScopedReferencePtr<TinyIATFunction>> m_hookFs;
		TinyArray<EXCLUDEDMODULE>	m_excludes;
	};
	/// <summary>
	/// APIº¯Êý
	/// </summary>
	class TinyIATFunction : public TinyReference < TinyIATFunction >
	{
		friend class TinyIATHook;
	public:
		TinyIATFunction(TinyIATHook* pAPIHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook);
		~TinyIATFunction();
		BOOL InstallHook();
		BOOL UninstallHook();
	private:
		BOOL DoInstallHook(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);
		BOOL DoInstallHook(HMODULE hmodCaller, LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);
	private:
		BOOL				m_bInstall;
		PROC				m_pfnOrig;
		PROC				m_pfnHook;
		CHAR				m_pzFunctionName[MAX_PATH];
		CHAR				m_pzCalleeModName[MAX_PATH];
		TinyLock			m_lock;
		TinyIATHook*		m_pIATHook;
		static PVOID        m_pMaximumApplicationAddress;
	};
	SELECTANY PVOID TinyIATFunction::m_pMaximumApplicationAddress = NULL;

#define JMP_64_SIZE            14
#define JMP_32_SIZE            5
	/// <summary>
	/// Ðé±íHook
	/// </summary>
	class TinyDetour
	{
	public:
		TinyDetour();
		~TinyDetour();
		BOOL	Initialize(FARPROC pfnOrig, FARPROC pfnNew);
		BOOL	BeginDetour();
		BOOL	EndDetour();
		BOOL	IsValid() const;
		FARPROC GetOrig() const;
	protected:
		FARPROC	m_pOrig;
		FARPROC	m_pTarget;
		DWORD	m_dwOrigProtect;
		BYTE	m_data[5];
		BOOL	m_bDetour;
	};
	inline FARPROC GetVTable(LPVOID ptr, UINT funcOffset)
	{
		ULONG *vtable = *(ULONG**)ptr;
		return (FARPROC)(*(vtable + funcOffset));
	}
	inline void SetVTable(LPVOID ptr, UINT funcOffset, FARPROC funcAddress)
	{
		ULONG *vtable = *(ULONG**)ptr;
		DWORD oldProtect;
		if (!VirtualProtect((LPVOID)(vtable + funcOffset), sizeof(ULONG), PAGE_EXECUTE_READWRITE, &oldProtect))
			return;
		*(vtable + funcOffset) = (ULONG)funcAddress;
	}
}
