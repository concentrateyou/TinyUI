#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include "TinyString.h"

#if defined(_WIN64)
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


namespace TinyFramework
{
#pragma pack(push, 1)
	/// <summary>
	/// 短跳
	/// </summary>
	typedef struct tagJMP_REL_SHORT
	{
		UINT8  opcode;
		UINT8  operand;
	} JMP_REL_SHORT, *PJMP_REL_SHORT;

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

#define JMP_SIZE 5

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
#if defined(_WIN64)
#define MEMORY_SIZE 64
#else
#define MEMORY_SIZE 32
#endif
#define MEMORY_BLOCK_SIZE	0x1000
#define MAX_MEMORY_RANGE	0x40000000
	/// <summary>
	/// 内联Hook(参考mini hook)
	/// </summary>
	class TinyDetour
	{
	public:
		TinyDetour();
		~TinyDetour();
		BOOL	Initialize(LPVOID pSRC, LPVOID pDST);
		BOOL	Uninitialize();
		BOOL	BeginDetour();
		BOOL	EndDetour();
		BOOL	IsEmpty() const;
		LPVOID	GetOrig() const;
	protected:
		BOOL	m_bAbove;
		BOOL	m_bDetour;
		BYTE	m_backup[16];
		LPVOID	m_lpSRC;
		LPVOID	m_lpDST;
		LPVOID	m_lpRelay;
		LPVOID	m_pTrampoline;
	};
	inline FARPROC GetVTable(LPVOID ptr, UINT funcOffset)
	{
		ULONG_PTR *vtable = *(ULONG_PTR**)ptr;
		return (FARPROC)(*(vtable + funcOffset));
	}
	inline void SetVTable(LPVOID ptr, UINT funcOffset, FARPROC funcAddress)
	{
		ULONG_PTR *vtable = *(ULONG_PTR**)ptr;
		DWORD oldProtect;
		if (!VirtualProtect((LPVOID)(vtable + funcOffset), sizeof(ULONG_PTR), PAGE_EXECUTE_READWRITE, &oldProtect))
			return;
		*(vtable + funcOffset) = (ULONG_PTR)funcAddress;
	}
}
