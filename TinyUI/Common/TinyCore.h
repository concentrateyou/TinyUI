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
		~TinyCriticalSection();
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
	class TinyEvent : public TinyObject
	{
		DECLARE_DYNAMIC(TinyEvent);
		DISALLOW_COPY_AND_ASSIGN(TinyEvent);
	public:
		TinyEvent();
		~TinyEvent();
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
	class TinyMutex : public TinyObject
	{
		DECLARE_DYNAMIC(TinyMutex);
		DISALLOW_COPY_AND_ASSIGN(TinyMutex);
	public:
		TinyMutex();
		~TinyMutex();
		BOOL Create(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
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
	class TinyLock : public TinyObject
	{
		DECLARE_DYNAMIC(TinyLock)
		DISALLOW_COPY_AND_ASSIGN(TinyLock)
	public:
		TinyLock();
		~TinyLock();
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
		~TinyAutoLock();
	private:
		TinyLock& m_lock;
	};
	/// <summary>
	/// Library封装 
	/// </summary>
	class TinyScopedLibrary
	{
		DISALLOW_COPY_AND_ASSIGN(TinyScopedLibrary);
	public:
		TinyScopedLibrary(LPCSTR pzName);
		~TinyScopedLibrary();
		BOOL IsValid() const;
		operator HINSTANCE() const;
		HINSTANCE Handle() const;
	private:
		HINSTANCE	m_hInstance;
	};
};

