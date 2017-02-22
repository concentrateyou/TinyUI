#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include "TinyString.h"

namespace TinyUI
{
	class TinyIATFunction;
	class TinyIATHook;
	/// <summary>
	/// IAT注入
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
	/// API函数
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

	static const BYTE jumps[] =
	{
		0x50,                                              //push rax
		0x48,0xB8,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, //mov rax,target_addr
		0x50,                                              //push rax
		0x48,0x8B,0x44,0x24,0x08,                          //mov rax,qword ptr ss:[rsp+8]
		0xC2,0x08,0x00                                     //ret 8
	};

	/// <summary>
	/// 内联Hook
	/// </summary>
	class TinyInlineHook
	{

	};
#define JMP_64_SIZE            14
#define JMP_32_SIZE            5
	/// <summary>
	/// 虚表Hook
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
		FARPROC	m_pfnOrig;
		FARPROC	m_pfnNew;
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
