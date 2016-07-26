#pragma once
#include "D3DCommon.h"
#include "D3DCore.h"

class CD3DDetour
{
public:
	CD3DDetour();
	~CD3DDetour();
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