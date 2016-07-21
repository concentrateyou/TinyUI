#pragma once
#include <Windows.h>
#include <Shellapi.h>
#include <Commdlg.h>
#include <comdef.h>
#include <Mmsystem.h>
#include <new.h>
#include <WinGDI.h>
#include <GdiPlus.h>
#include <Shlwapi.h> 
#include <commctrl.inl>
#include <Psapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <crtdbg.h>
#include <utility>
#include <algorithm>
#include <limits>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")

#ifndef ASSERT
#define ASSERT(expr) _ASSERTE(expr)
#endif 
void Trace(LPCTSTR lpszFormat, ...);
#ifndef TRACE
#define TRACE Trace
#endif
#ifndef ASSUME
#define ASSUME(expr) do { ASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif

#define IS_VALID_STRING_PTR(ptr, type) \
	(!IsBadStringPtr((ptr), (UINT)-1))

#define DISALLOW_COPY(TypeName) \
private:\
	TypeName(const TypeName&) = delete;

#define DISALLOW_ASSIGN(TypeName) \
private:\
	void operator=(const TypeName&) = delete;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
private:\
	TypeName(const TypeName&) = delete;  \
	void operator=(const TypeName&) = delete;

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
private:\
	TypeName() = delete;   \
	DISALLOW_COPY_AND_ASSIGN(TypeName);

template<typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define ARRAYSIZE_UNSAFE(a) \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

/// <summary>
/// 高精度计数器
/// </summary>
class CPerformanceTimer
{
public:
	CPerformanceTimer();
	~CPerformanceTimer();
	LONGLONG GetMicroseconds();
	void Start();
	void Stop();
private:
	BOOL			m_bRunning;
	DWORD			m_dwTimerMask;
	DWORD			m_dwTick;
	LONGLONG		m_lastTime;
	LARGE_INTEGER	m_lFrequency;
	LARGE_INTEGER	m_startPerformanceCount;
};