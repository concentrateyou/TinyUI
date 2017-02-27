#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include "TinyString.h"

#ifdef _WIN64
#include "../HDE/hde64.h"
typedef hde64s HDE;
#define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#define MEMORY_SLOT_SIZE 64
#define TRAMPOLINE_MAX_SIZE (MEMORY_SLOT_SIZE - sizeof(JMP_ABS))
#else
#include "../HDE/hde32.h"
typedef hde32s HDE;
#define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#define MEMORY_SLOT_SIZE 32
#define TRAMPOLINE_MAX_SIZE MEMORY_SLOT_SIZE
#endif

namespace TinyUI
{
#pragma pack(push, 1)
	/// <summary>
	/// 相对跳转
	/// </summary>
	typedef struct tagJMP_REL
	{
		UINT8  opcode;
		UINT32 operand;
	} JMP_REL, *PJMP_REL, CALL_REL;
	/// <summary>
	/// 绝对跳转
	/// </summary>
	typedef struct tagJMP_ABS
	{
		UINT8  opcode0;
		UINT8  opcode1;
		UINT32 dummy;
		UINT64 address;
	} JMP_ABS, *PJMP_ABS;
	/// <summary>
	/// 绝对Call
	/// </summary>
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

#ifdef _WIN64
#define JMP_SIZE            14
#else
#define JMP_SIZE            5
#endif // _WIN64
	/// <summary>
	/// 虚表Hook
	/// </summary>
	class TinyDetour
	{
	public:
		TinyDetour();
		~TinyDetour();
		BOOL	Initialize(FARPROC lpSRC, FARPROC lpDST);
		BOOL	BeginDetour();
		BOOL	EndDetour();
		BOOL	IsValid() const;
		FARPROC GetOrig() const;
	protected:
		FARPROC	m_lpSRC;
		FARPROC	m_lpDST;
		DWORD	m_dwProtect;
		BYTE	m_data[JMP_SIZE];
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
	/// <summary>
	/// 内联Hook
	/// </summary>
	class TinyInlineHook
	{
	public:
		TinyInlineHook();
		~TinyInlineHook();
		BOOL	Initialize(LPVOID pSRC, LPVOID pDST);
		BOOL	Uninitialize();
		BOOL	BeginDetour();
		BOOL	EndDetour();
		BOOL	IsValid() const;
		LPVOID	GetOrig() const;
	protected:
		BOOL	m_bDetour;
		BYTE	m_backup[16];
		LPVOID	m_lpSRC;
		LPVOID	m_lpDST;
		LPVOID	m_pTrampoline;
	};
}
