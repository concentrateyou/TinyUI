#pragma once
#include "TinyObject.h"
#include "TinyString.h"
#include "TinyCallback.h"

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
#ifdef _WIN32
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
#endif // _WIN32
	/// <summary>
	/// 条件变量
	/// </summary>
	class TinyConditionVariable : public TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinyConditionVariable)
	public:
		TinyConditionVariable();
		~TinyConditionVariable();
		BOOL Lock(CRITICAL_SECTION& cs, DWORD dwMS = INFINITE);
		BOOL Lock(SRWLOCK& lock, DWORD dwMS = INFINITE);
		void Unlock(BOOL bAll);
	private:
		CONDITION_VARIABLE	m_cv;
		CRITICAL_SECTION	m_cs;
		SRWLOCK				m_lock;
		BOOL				m_allowSRW;
	};
	/// <summary>
	/// 异步等待
	/// </summary>
	class TinyWaiter
	{
		DISALLOW_COPY_AND_ASSIGN(TinyWaiter)
	public:
		TinyWaiter();
		~TinyWaiter();
	public:
		BOOL IsValid() const;
		BOOL RegisterOnce(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback);
		BOOL Register(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback);
		BOOL Unregister();
	private:
		BOOL Register(HANDLE handle, DWORD dwMS, DWORD dwFlag, Callback<void(BOOLEAN)>&& callback);
	private:
		static void CALLBACK WaitOrTimerCallback(PVOID ps, BOOLEAN b);
	private:
		HANDLE		m_hWaitHandle;
		HANDLE		m_handle;
		Callback<void(BOOLEAN)> m_callback;
	};

	/// <summary>
	/// 计数器队列
	/// </summary>
	class TinyTimerQueue
	{
		DISALLOW_COPY_AND_ASSIGN(TinyTimerQueue)
	public:
		TinyTimerQueue();
		~TinyTimerQueue();
		BOOL Create();
		void Destory();
		HANDLE Register(WAITORTIMERCALLBACK callback, LPVOID ps, WORD dueTime, DWORD period, DWORD dwFlag = WT_EXECUTEINTIMERTHREAD);
		BOOL Change(HANDLE hTimer, WORD dueTime, DWORD period);
		BOOL Unregister(HANDLE hTimer);
	private:
		HANDLE m_handle;
	};
	/// <summary>
	/// 高性能定时器
	/// </summary>
	class TinyTimer
	{
		DISALLOW_COPY_AND_ASSIGN(TinyTimer)
	public:
		TinyTimer();
		~TinyTimer();
		BOOL SetCallback(INT delay, Closure&& callback);
		BOOL Wait(INT delay, DWORD dwMilliseconds);
		void Close();
	private:
		UINT		m_timerID;
		Closure		m_callback;
		TinyEvent	m_event;
	private:
		static void CALLBACK TimerCallback(UINT uTimerID, UINT  uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
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
	//////////////////////////////////////////////////////////////////////////
	void WINAPI SleepMS(DWORD dwMS);
};

