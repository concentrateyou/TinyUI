#pragma once
#include "TinyObject.h"
#include "TinyString.h"

/// <summary>
/// 信号量对象
/// </summary>
namespace TinyUI
{
	/// <summary>
	/// 临界区
	/// </summary>
	class TinyLock
	{
		DISALLOW_COPY_AND_ASSIGN(TinyLock)
	public:
		TinyLock() throw();
		virtual ~TinyLock();
		operator CRITICAL_SECTION& ();
		BOOL Lock() throw();
		void Unlock() throw();
	private:
		CRITICAL_SECTION m_s;
	};
	/// <summary>
	/// 事件
	/// </summary>
	class TinyEvent
	{
		DISALLOW_COPY_AND_ASSIGN(TinyEvent)
	public:
		TinyEvent();
		virtual ~TinyEvent();
		operator HANDLE() const;
		HANDLE Handle() const;
		BOOL CreateEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
		BOOL CreateEventEx(LPSECURITY_ATTRIBUTES lpEventAttributes = NULL, LPCTSTR   lpName = NULL, DWORD  dwFlags = 0, DWORD  dwDesiredAccess = EVENT_MODIFY_STATE | SYNCHRONIZE);
		BOOL OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
		BOOL SetEvent();
		BOOL PulseEvent();
		BOOL ResetEvent();
		BOOL Lock(DWORD dwTimeout = INFINITE);
		BOOL Unlock();
		void Close();
	private:
		HANDLE		m_hEvent;

	};
	/// <summary>
	/// 互斥量
	/// </summary>
	class TinyMutex
	{
		DISALLOW_COPY_AND_ASSIGN(TinyMutex)
	public:
		TinyMutex();
		virtual ~TinyMutex();
		BOOL Create(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
		BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
		operator HANDLE() const;
		HANDLE Handle() const;
		BOOL Lock(DWORD dwTimeout = INFINITE);
		BOOL Unlock();
		void Close();
	private:
		HANDLE  m_hMutex;
	};
	/// <summary>
	/// 自动锁
	/// </summary>
	class TinyAutoLock
	{
		DISALLOW_COPY_AND_ASSIGN(TinyAutoLock)
	public:
		explicit TinyAutoLock(TinyLock& lock);
		virtual ~TinyAutoLock();
	private:
		TinyLock& m_lock;
	};
	/// <summary>
	/// 自旋锁
	/// </summary>
	class TinySpinLock : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinySpinLock)
	public:
		TinySpinLock();
		virtual ~TinySpinLock();
		BOOL Initialize(DWORD dwSpinCount = 0) throw();
		void Uninitialize();
		void Lock();
		BOOL TryLock();
		void Unlock();
	private:
		CRITICAL_SECTION m_s;
	};
	/// <summary>
	/// 读写锁
	/// </summary>
	class TinySRWLock : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinySRWLock)
	public:
		TinySRWLock();
		virtual ~TinySRWLock();
		operator SRWLOCK&();
		void Lock(BOOL write);
		void Unlock(BOOL write);
	private:
		SRWLOCK	 m_SRW;
	};
	/// <summary>
	/// 信号量
	/// </summary>
	class TinySemaphore : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinySemaphore)
	public:
		TinySemaphore();
		BOOL Create(LONG lInitialCount = 1, LONG lMaxCount = 1, LPCTSTR pstrName = NULL, LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
		BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
		operator HANDLE() const;
		HANDLE Handle() const;
		BOOL Lock(DWORD dwTimeout = INFINITE);
		BOOL Unlock(LONG lCount, LPLONG lprevCount = NULL);
	public:
		virtual ~TinySemaphore();
	private:
		HANDLE  m_hSemaphore;
	};
	/// <summary>
	/// 高性能锁
	/// </summary>
	class TinyPerformanceLock : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinyPerformanceLock)
	public:
		TinyPerformanceLock();
		~TinyPerformanceLock();
	public:
		void Lock(LONG value = 1, UINT spin = 2048);
		void Unlock();
	private:
		SYSTEM_INFO  m_si;
		volatile LONG* m_lock;
	};
	/// <summary>
	/// 条件变量
	/// </summary>
	class TinyConditionVariable : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinyConditionVariable)
	public:
		TinyConditionVariable(CRITICAL_SECTION& cs);
		TinyConditionVariable(SRWLOCK& lock);
		~TinyConditionVariable();
		BOOL Lock(DWORD dwMS = INFINITE);
		void Unlock(BOOL bAll);
	private:
		CONDITION_VARIABLE	m_cv;
		CRITICAL_SECTION	m_cs;
		SRWLOCK				m_lock;
		BOOL				m_allowSRW;
	};
	/// <summary>
	/// Library封装 
	/// </summary>
	class TinyScopedLibrary
	{
		DISALLOW_COPY_AND_ASSIGN(TinyScopedLibrary)
	public:
		TinyScopedLibrary();
		explicit TinyScopedLibrary(LPCSTR pzName);
		~TinyScopedLibrary();
		BOOL IsValid() const;
		void Reset(HINSTANCE hInstance);
		void Reset(LPCSTR pzName);
		FARPROC GetFunctionPointer(LPCSTR lpProcName) const;
		operator HINSTANCE() const;
		HINSTANCE Handle() const;
	private:
		HINSTANCE	m_hInstance;
	};
};

