#pragma once
#include "D3DCommon.h"
#include "D3DCore.h"

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
	UINT		CaptureType;
	DWORD		Format;
	UINT		Width;
	UINT		Height;
	BOOL		bFlip;
	UINT		Pitch;
	UINT		MapID;
	DWORD		MapSize;
	HWND		HwndCapture;
}CaptureShare;

#pragma pack(push, 8)

typedef struct tagSharedTextureData
{
	LONGLONG    FrameTime;
	HANDLE      TextureHandle;
}SharedTextureData;

#pragma pack(pop)

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