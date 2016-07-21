#pragma once
#include "D3DCommon.h"

/// <summary>
/// 临界区
/// </summary>
class CCriticalSection
{
	DISALLOW_COPY_AND_ASSIGN(CCriticalSection);
public:
	CCriticalSection() throw();
	~CCriticalSection();
	HRESULT Lock() throw();
	HRESULT Unlock() throw();
	BOOL Try() throw();
	HRESULT Initialize() throw();
	HRESULT Uninitialize() throw();
private:
	CRITICAL_SECTION section;
};
/// <summary>
/// 事件
/// </summary>
class CEvent
{
	DISALLOW_COPY_AND_ASSIGN(CEvent);
public:
	CEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	~CEvent();
	operator HANDLE() const;
	HANDLE Handle();
	BOOL CreateEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	BOOL OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
	BOOL SetEvent();
	BOOL PulseEvent();
	BOOL ResetEvent();
	BOOL Lock(DWORD dwTimeout = INFINITE);
	BOOL Unlock();
private:
	HANDLE  m_hEvent;
};
/// <summary>
/// 互斥量
/// </summary>
class CMutex
{
	DISALLOW_COPY_AND_ASSIGN(CMutex);
public:
	CMutex(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	~CMutex();
	BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
	operator HANDLE() const;
	HANDLE Handle();
	BOOL Lock(DWORD dwTimeout = INFINITE);
	BOOL Unlock();
private:
	HANDLE  m_hMutex;
};
/// <summary>
/// 平台锁
/// </summary>
class CLock
{
	DISALLOW_COPY_AND_ASSIGN(CLock)
public:
	CLock();
	~CLock();
	void Acquire();
	void Release();
	BOOL Try();
private:
	CCriticalSection m_section;
};

