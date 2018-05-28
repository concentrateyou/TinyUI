#pragma once
#include "TinyCommon.h"

namespace TinyUI
{
#pragma push_macro("new")
#undef new
#if defined(_M_IX86)
	inline PVOID WINAPI __AllocThunk(VOID);
	inline VOID WINAPI __FreeThunk(PVOID ptr);
#pragma pack(push,1)
	struct _callthunk
	{
		DWORD   m_mov;
		DWORD   m_this;
		BYTE    m_jmp;
		DWORD   m_proc;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_mov = 0x042444C7;  //C7 44 24 0C
			m_this = PtrToUlong(pThis);
			m_jmp = 0xe9;
			m_proc = DWORD((INT_PTR)proc - ((INT_PTR)this + sizeof(_callthunk)));
			return FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
		}

		void*	GetCodeAddress()
		{
			//GetCodeAddress获得的函数pProc是从DWORD m_mov开始执行
			return this;
		}
		void* operator new(size_t)
		{
			return __AllocThunk();
		}
		void operator delete(void* pThunk)
		{
			__FreeThunk(pThunk);
		}
	};
#pragma pack(pop)
#elif defined(_M_AMD64)
	inline PVOID WINAPI __AllocThunk(VOID);
	inline VOID WINAPI __FreeThunk(PVOID ptr);
#pragma pack(push,2)
	struct _callthunk
	{
		USHORT  RcxMov;
		ULONG64 RcxImm;
		USHORT  RaxMov;
		ULONG64 RaxImm;
		USHORT  RaxJmp;
		BOOL Initialize(DWORD_PTR proc, void *pThis)
		{
			RcxMov = 0xb948;
			RcxImm = (ULONG64)pThis;
			RaxMov = 0xb848;
			RaxImm = (ULONG64)proc;
			RaxJmp = 0xe0ff;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void* GetCodeAddress()
		{
			return this;
		}
		void* operator new(size_t)
		{
			return __AllocThunk();
		}
		void operator delete(void* pThunk)
		{
			__FreeThunk(pThunk);
		}
	};
#pragma pack(pop)
#elif defined(_SH3_)
#pragma pack(push,4)
	struct _callthunk
	{
		WORD	m_mov_r0;
		WORD	m_mov_r1;
		WORD	m_jmp;
		WORD	m_nop;
		DWORD	m_pFunc;
		DWORD	m_pThis;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_mov_r0 = 0xd001;
			m_mov_r1 = 0xd402;
			m_jmp = 0x402b;
			m_nop = 0x0009;
			m_pFunc = (DWORD)proc;
			m_pThis = (DWORD)pThis;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void*	GetCodeAddress()
		{
			return this;
		}
	};
#pragma pack(pop)
#elif defined(_MIPS_)
#pragma pack(push,4)
	struct _callthunk
	{
		WORD	m_pFuncHi;
		WORD	m_lui_t0;
		WORD	m_pFuncLo;
		WORD	m_ori_t0;
		WORD	m_pThisHi;
		WORD	m_lui_a0;
		DWORD	m_jr_t0;
		WORD	m_pThisLo;
		WORD	m_ori_a0;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_pFuncHi = HIWORD(proc);
			m_lui_t0 = 0x3c08;
			m_pFuncLo = LOWORD(proc);
			m_ori_t0 = 0x3508;
			m_pThisHi = HIWORD(pThis);
			m_lui_a0 = 0x3c04;
			m_jr_t0 = 0x01000008;
			m_pThisLo = LOWORD(pThis);
			m_ori_a0 = 0x3484;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void*	GetCodeAddress()
		{
			return this;
		}
	};
#pragma pack(pop)
#elif defined (_M_THUMB)
	inline PVOID WINAPI __AllocThunk(VOID);
	inline VOID WINAPI __FreeThunk(PVOID ptr);
#pragma pack(push,2)
	struct _callthunk
	{
		USHORT	m_mov_r0[2];
		USHORT	m_mov_pc[2];
		DWORD	m_pThis;
		DWORD	m_pFunc;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_mov_r0[0] = 0xF8DF;
			m_mov_r0[1] = 0x0004;
			m_mov_pc[0] = 0xF8DF;
			m_mov_pc[1] = 0xF004;
			m_pThis = (DWORD)pThis;
			m_pFunc = (DWORD)proc;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void*	GetCodeAddress()
		{
			return (void *)((ULONG_PTR)this | 1);
		}
		void* operator new(size_t)
		{
			return __AllocThunk();
		}
		void operator delete(void* pThunk)
		{
			__FreeThunk(pThunk);
		}
	};
#pragma pack(pop)
#elif defined(_M_ARM64)
	PVOID __stdcall __AllocThunk(VOID);
	VOID  __stdcall __FreeThunk(PVOID);
#pragma pack(push,4)  
	struct _callthunk
	{
		ULONG   m_ldr_r16;
		ULONG   m_ldr_r0;
		ULONG   m_br;
		ULONG   m_pad;
		ULONG64	m_pThis;
		ULONG64	m_pFunc;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_ldr_r16 = 0x580000D0;
			m_ldr_r0 = 0x58000060;
			m_br = 0xd61f0200;
			m_pThis = (ULONG64)pThis;
			m_pFunc = (ULONG64)proc;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void* GetCodeAddress()
		{
			return (void *)((ULONG_PTR)this | 1);
		}
		void* operator new(size_t)
		{
			return __AllocThunk();
		}
		void operator delete(void* pThunk)
		{
			__FreeThunk(pThunk);
		}
	};
#pragma pack(pop)
#elif defined(_ARM_)
#pragma pack(push,4)
	struct _callthunk
	{
		DWORD	m_mov_r0;
		DWORD	m_mov_pc;
		DWORD	m_pThis;
		DWORD	m_pFunc;
		BOOL	Initialize(DWORD_PTR proc, void* pThis)
		{
			m_mov_r0 = 0xE59F0000;
			m_mov_pc = 0xE59FF000;
			m_pThis = (DWORD)pThis;
			m_pFunc = (DWORD)proc;
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void* GetCodeAddress()
		{
			return this;
		}
	};
#pragma pack(pop)
#elif defined(_M_IA64)
#pragma pack(push,8)
	extern "C" void _CallThunkProc(void);
	struct _FuncDesc
	{
		void* pfn;
		void* gp;
	};
	struct _callthunk
	{
		_FuncDesc m_funcdesc;
		void* m_pFunc;
		void* m_pThis;

		BOOL Initialize(DWORD_PTR proc, void* pThis)
		{
			m_funcdesc.pfn = ((_FuncDesc*)(&_CallThunkProc))->pfn;
			m_funcdesc.gp = &m_pFunc;
			m_pFunc = reinterpret_cast<void*>(proc);
			m_pThis = pThis;
			::FlushInstructionCache(GetCurrentProcess(), this, sizeof(_callthunk));
			return TRUE;
		}
		void* GetCodeAddress()
		{
			return(&m_funcdesc);
		}
	};
#pragma pack(pop)
#else
#error Only ARM, ARM64, ALPHA, SH3, MIPS, IA64, AMD64 and X86 supported
#endif

	inline PVOID WINAPI __AllocThunk(VOID)
	{
		LPVOID address = VirtualAlloc(0, sizeof(_callthunk), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!address) return NULL;
		return address;
	}
	inline VOID WINAPI __FreeThunk(PVOID ptr)
	{
		if (!ptr) return;
		VirtualFree(ptr, 0, MEM_RELEASE);
	}


#pragma pack(push,8)
	class TinyThunk
	{
	public:
		_callthunk *pThunk;
		TinyThunk();
		~TinyThunk();
		BOOL Initialize(DWORD_PTR proc, void *pThis);
		void* GetCodeAddress();
	};
#pragma pack(pop)
	class TinyLoopThunk
	{
	public:
		TinyThunk thunk;
		BOOL Initialize(WNDPROC proc, void* pThis);
		WNDPROC GetWNDPROC();
	};
#pragma pop_macro("new")
}



