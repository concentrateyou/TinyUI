#include "stdafx.h"
#include "D3DCommon.h"

typedef signed long  SLONG;

void Trace(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	INT nBuf;
	TCHAR szBuffer[512];

#pragma warning(push)//保存当前的编译器警告状态
#pragma warning(disable : 4996)
	nBuf = _vsntprintf(szBuffer, ARRAYSIZE(szBuffer), lpszFormat, args);
#pragma warning(pop)//恢复原先的警告状态

	ASSERT(nBuf >= 0);

	OutputDebugString(szBuffer);

	va_end(args);
}
//////////////////////////////////////////////////////////////////////////
CPerformanceTimer::CPerformanceTimer()
	:m_dwTick(0),
	m_dwTimerMask(0),
	m_lastTime(0),
	m_bRunning(FALSE)
{
	QueryPerformanceFrequency(&m_lFrequency);
}
CPerformanceTimer::~CPerformanceTimer()
{

}
void CPerformanceTimer::BeginTime()
{
	m_dwTimerMask = 1;
	DWORD dwProcessMask = 0;
	DWORD dwSystemMask = 0;
#if _MSC_VER >= 1400 && defined (_M_X64)
	GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&dwProcessMask, (PDWORD_PTR)&dwSystemMask);
#else
	GetProcessAffinityMask(GetCurrentProcess(), &dwProcessMask, &dwSystemMask);
#endif
	if (dwProcessMask == 0)
		dwProcessMask = 1;

	while ((m_dwTimerMask & dwProcessMask) == 0)
	{
		m_dwTimerMask <<= 1;
	}
	HANDLE hHandle = GetCurrentThread();
	DWORD oldMask = SetThreadAffinityMask(hHandle, m_dwTimerMask);
	QueryPerformanceFrequency(&m_lFrequency);
	QueryPerformanceCounter(&m_startPerformanceCount);
	m_dwTick = GetTickCount();
	SetThreadAffinityMask(hHandle, oldMask);
}

void CPerformanceTimer::EndTime()
{
	HANDLE hHandle = GetCurrentThread();
	DWORD oldMask = SetThreadAffinityMask(hHandle, m_dwTimerMask);
	LARGE_INTEGER currentPerformanceCount;
	QueryPerformanceCounter(&currentPerformanceCount);
	SetThreadAffinityMask(hHandle, oldMask);
	LONGLONG elapseTime = currentPerformanceCount.QuadPart - currentPerformanceCount.QuadPart;
	LONGLONG elapseTicks = (LONGLONG)(1000 * elapseTime / m_lFrequency.QuadPart);
	ULONG check = GetTickCount() - m_dwTick;
	SLONG offset = (SLONG)(elapseTicks - check);
	if (offset < -100 || offset > 100)
	{
		LONGLONG adjustment = std::min<LONGLONG>(offset * m_lFrequency.QuadPart / 1000, elapseTime - m_lastTime);
		m_startPerformanceCount.QuadPart += adjustment;
		elapseTime -= adjustment;
	}
	m_lastTime = elapseTime;
}

LONGLONG CPerformanceTimer::GetMicroseconds()
{
	LONGLONG ticks = (1000000 * m_lastTime / m_lFrequency.QuadPart);
	return ticks;
}
//////////////////////////////////////////////////////////////////////////
CScopedLibrary::CScopedLibrary(const string& str)
{
	m_hInstance = ::LoadLibrary(str.c_str());
}
CScopedLibrary::~CScopedLibrary()
{
	if (m_hInstance)
	{
		::FreeLibrary(m_hInstance);
		m_hInstance = NULL;
	}
}
CScopedLibrary::operator HINSTANCE() const
{
	return m_hInstance;
}

HINSTANCE CScopedLibrary::Handle() const
{
	return m_hInstance;
}

BOOL CScopedLibrary::IsValid() const
{
	return m_hInstance != NULL;
}


