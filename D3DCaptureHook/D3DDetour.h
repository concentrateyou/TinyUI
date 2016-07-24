#pragma once
#include "D3DCommon.h"
#include "D3DSemaphore.h"

class D3DDetour
{
public:
	D3DDetour();
	~D3DDetour();
	BOOL	Initialize(FARPROC pfnOrig, FARPROC pfnHook);
	BOOL	BeginDetour();
	BOOL	EndDetour();
	BOOL	IsValid() const;
	FARPROC GetOrig() const;
protected:
	FARPROC	m_pfnOrig;
	FARPROC	m_pfnHook;
	DWORD	m_dwOrigProtect;
	BYTE	m_data[5];
	BOOL	m_bHook;
};
#define CAPTURETYPE_MEMORY      1
#define CAPTURETYPE_SHAREDTEX   2
typedef struct tagCaptureEntry
{
	UINT    captureType;
	DWORD   format;
	UINT    cx;
	UINT	cy;
	BOOL    bFlip;
	UINT    pitch;
	UINT    mapID;
	DWORD   mapSize;
	HWND	hwndCapture;
}CaptureEntry;
/// <summary>
/// ÎÆÀíÊý¾Ý
/// </summary>
typedef struct tagSharedTextureData
{
	LONGLONG    frameTime;
	DWORD       textureHandle;
}SharedTextureData;

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