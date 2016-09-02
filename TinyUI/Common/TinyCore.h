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
		DECLARE_DYNAMIC(TinyCriticalSection);
		DISALLOW_COPY_AND_ASSIGN(TinyCriticalSection);
	public:
		TinyCriticalSection() throw();
		virtual ~TinyCriticalSection();
		void Lock() throw();
		BOOL TryLock() throw();
		void Unlock() throw();
		void Initialize() throw();
		void Uninitialize() throw();
	private:
		CRITICAL_SECTION section;
	};
	/// <summary>
	/// 事件
	/// </summary>
	class TinyEvent : public TinyObject
	{
		DECLARE_DYNAMIC(TinyEvent);
		DISALLOW_COPY_AND_ASSIGN(TinyEvent);
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
		DECLARE_DYNAMIC(TinyMutex);
		DISALLOW_COPY_AND_ASSIGN(TinyMutex);
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
		void Acquire();
		void Release();
		BOOL Try();
	private:
		TinyCriticalSection m_section;
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
		DECLARE_DYNAMIC(TinySpinLock);
		DISALLOW_COPY_AND_ASSIGN(TinySpinLock);
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
	/// 信号量
	/// </summary>
	class TinySemaphore : public TinyObject
	{
		DECLARE_DYNAMIC(TinySemaphore);
		DISALLOW_COPY_AND_ASSIGN(TinySemaphore);
	public:
		explicit TinySemaphore();
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
	/// Library封装 
	/// </summary>
	class TinyScopedLibrary
	{
		DISALLOW_COPY_AND_ASSIGN(TinyScopedLibrary);
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

