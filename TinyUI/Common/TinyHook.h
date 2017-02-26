#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include "TinyString.h"

#ifdef _WIN64
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

#pragma pack(push, 1)
	typedef struct tagJMP_REL_SHORT
	{
		UINT8  opcode;
		UINT8  relative;
	} JMP_REL_SHORT, *PJMP_REL_SHORT;

	typedef struct tagJMP_REL
	{
		UINT8  opcode;
		UINT32 relative;
	} JMP_REL, *PJMP_REL, CALL_REL;

	typedef struct tagJMP_ABS
	{
		UINT8  opcode0;
		UINT8  opcode1;
		UINT32 dummy;
		UINT64 address;
	} JMP_ABS, *PJMP_ABS;

	typedef struct tagCALL_ABS
	{
		UINT8  opcode0;
		UINT8  opcode1;
		UINT32 dummy0;
		UINT8  dummy1;
		UINT8  dummy2;
		UINT64 address;
	} CALL_ABS;

	typedef struct tagJCC_REL
	{
		UINT8  opcode0;
		UINT8  opcode1;
		UINT32 operand;
	} JCC_REL;

	typedef struct tagJCC_ABS
	{
		UINT8  opcode;
		UINT8  dummy0;
		UINT8  dummy1;
		UINT8  dummy2;
		UINT32 dummy3;
		UINT64 address;
	} JCC_ABS;
#pragma pack(pop)
#define PAGE_EXECUTE_FLAGS \
    (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
	BOOL IsExecutableAddress(LPVOID pAddress);
	/// <summary>
	/// 内联
	/// </summary>
	class TinyInlineHook
	{
	public:
		BOOL Initialize(LPVOID pTarget, LPVOID pDetour);
	protected:
		LPVOID					m_pSRC;
		LPVOID					m_pDST;
		TinyScopedArray<BYTE>	m_data;
	};
}
