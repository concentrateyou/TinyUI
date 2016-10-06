#pragma once
#include "TinyObject.h"

/// <summary>
/// 信号量对象
/// </summary>
namespace TinyUI
{

	/// <summary>
	/// 临界区
	/// </summary>
	class TinyCriticalSection : public TinyObject
	{
		DECLARE_DYNAMIC(TinyCriticalSection)
		DISALLOW_COPY_AND_ASSIGN(TinyCriticalSection)
	public:
		TinyCriticalSection() throw();
		virtual ~TinyCriticalSection();
		BOOL Initialize() throw();
		BOOL Lock() throw();
		void Unlock() throw();
	private:
		CRITICAL_SECTION section;
	};
	/// <summary>
	/// 事件
	/// </summary>
	class TinyEvent : public TinyObject
	{
		DECLARE_DYNAMIC(TinyEvent)
		DISALLOW_COPY_AND_ASSIGN(TinyEvent)
	public:
		TinyEvent();
		virtual ~TinyEvent();
		operator HANDLE() const;
		HANDLE Handle() const;
		BOOL CreateEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
		BOOL OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName);
		BOOL SetEvent();
		BOOL PulseEvent();
		BOOL ResetEvent();
		BOOL Lock(DWORD dwTimeout = INFINITE);
		BOOL Unlock();
		void Close();
	private:
		HANDLE  m_hEvent;
	};
	/// <summary>
	/// 互斥量
	/// </summary>
	class TinyMutex : public TinyObject
	{
		DECLARE_DYNAMIC(TinyMutex)
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
	private:
		HANDLE  m_hMutex;
	};
	/// <summary>
	/// 平台锁
	/// </summary>
	class TinyLock : public TinyObject
	{
		DECLARE_DYNAMIC(TinyLock)
		DISALLOW_COPY_AND_ASSIGN(TinyLock)
	public:
		TinyLock();
		virtual ~TinyLock();
		BOOL Lock();
		void Unlock();
	private:
		TinyCriticalSection m_s;
	};
	/// <summary>
	/// 自动锁
	/// </summary>
	class TinyAutoLock : public TinyObject
	{
		DECLARE_DYNAMIC(TinyAutoLock)
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
		DECLARE_DYNAMIC(TinySpinLock)
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
		CRITICAL_SECTION section;
	};
	/// <summary>
	/// 读写锁
	/// </summary>
	class TinySRWLock : public TinyObject
	{
		DECLARE_DYNAMIC(TinySRWLock)
		DISALLOW_COPY_AND_ASSIGN(TinySRWLock)
	public:
		TinySRWLock();
		virtual ~TinySRWLock();
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
		DECLARE_DYNAMIC(TinySemaphore)
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
		DECLARE_DYNAMIC(TinyPerformanceLock)
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

