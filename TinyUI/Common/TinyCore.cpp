#include "../stdafx.h"
#include "TinyCore.h"
#include "TinyLogging.h"

namespace TinyUI
{
	/////////////////////////////////////////////////////////////////////////
	TinyLock::TinyLock() throw()
	{
		ZeroMemory(&m_s, sizeof(m_s));
		InitializeCriticalSection(&m_s);
	}
	TinyLock::~TinyLock()
	{
		::DeleteCriticalSection(&m_s);
	}
	BOOL TinyLock::Lock() throw()
	{
		::EnterCriticalSection(&m_s);
		return TRUE;
	}
	void TinyLock::Unlock() throw()
	{
		::LeaveCriticalSection(&m_s);
	}
	TinyLock::operator CRITICAL_SECTION& ()
	{
		return m_s;
	}
	/////////////////////////////////////////////////////////////////////////
	TinyEvent::TinyEvent()
		:m_hEvent(NULL)
	{

	}
	TinyEvent::~TinyEvent()
	{
		Close();
	}
	TinyEvent::operator HANDLE() const
	{
		return m_hEvent;
	}
	HANDLE TinyEvent::Handle() const
	{
		return m_hEvent;
	}
	BOOL TinyEvent::CreateEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR lpszNAme, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hEvent = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, lpszNAme);
		//LOG(INFO) << "TinyEvent-CreateEvent:" << m_hEvent << "\n";
		return m_hEvent == NULL ? FALSE : TRUE;
	}
	BOOL TinyEvent::CreateEventEx(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCTSTR lpName, DWORD  dwFlags, DWORD  dwDesiredAccess)
	{
		m_hEvent = ::CreateEventEx(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
		return m_hEvent == NULL ? FALSE : TRUE;
	}
	BOOL TinyEvent::OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		m_hEvent = ::OpenEvent(dwDesiredAccess, bInheritHandle, lpName);
		return m_hEvent == NULL ? FALSE : TRUE;
	}
	BOOL TinyEvent::SetEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::SetEvent(m_hEvent);
	}
	BOOL TinyEvent::PulseEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::PulseEvent(m_hEvent);
	}
	BOOL TinyEvent::ResetEvent()
	{
		ASSERT(m_hEvent != NULL);
		return ::ResetEvent(m_hEvent);
	}
	BOOL TinyEvent::Lock(DWORD dwTimeout)
	{
		ASSERT(m_hEvent != NULL);
		//LOG(INFO) << "TinyEvent-Lock:" << m_hEvent << "\n";
		DWORD dwRet = ::WaitForSingleObject(m_hEvent, dwTimeout);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
			return TRUE;
		else
			return FALSE;
	}
	BOOL TinyEvent::Unlock()
	{
		return TRUE;
	}
	void TinyEvent::Close()
	{
		if (m_hEvent != NULL)
		{
			::CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}
	/////////////////////////////////////////////////////////////////////////
	TinyMutex::TinyMutex()
		:m_hMutex(NULL)
	{

	}
	TinyMutex::~TinyMutex()
	{
		Close();
	}
	BOOL TinyMutex::Create(BOOL bInitiallyOwn, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hMutex = ::CreateMutex(lpsaAttribute, bInitiallyOwn, lpszName);
		return m_hMutex == NULL ? FALSE : TRUE;
	}
	BOOL TinyMutex::Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		m_hMutex = ::OpenMutex(dwDesiredAccess, bInheritHandle, lpName);
		return m_hMutex == NULL ? FALSE : TRUE;
	}
	TinyMutex::operator HANDLE() const
	{
		return (HANDLE)(this == NULL ? NULL : m_hMutex);
	}
	HANDLE TinyMutex::Handle() const
	{
		return m_hMutex;
	}
	BOOL TinyMutex::Lock(DWORD dwTimeout)
	{
		ASSERT(m_hMutex != NULL);
		DWORD dwRet = ::WaitForSingleObject(m_hMutex, dwTimeout);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
			return TRUE;
		else
			return FALSE;
	}
	BOOL TinyMutex::Unlock()
	{
		ASSERT(m_hMutex != NULL);
		return ::ReleaseMutex(m_hMutex);
	}
	void TinyMutex::Close()
	{
		if (m_hMutex != NULL)
		{
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyAutoLock::TinyAutoLock(TinyLock& lock) : m_lock(lock)
	{
		m_lock.Lock();
	}
	TinyAutoLock::~TinyAutoLock()
	{
		m_lock.Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	TinySpinLock::TinySpinLock()
	{
		ZeroMemory(&m_s, sizeof(m_s));
	}
	TinySpinLock::~TinySpinLock()
	{
		Uninitialize();
	}
	BOOL TinySpinLock::Initialize(DWORD dwSpinCount) throw()
	{
		return InitializeCriticalSectionAndSpinCount(&m_s, dwSpinCount);
	}
	void TinySpinLock::Uninitialize()throw()
	{
		DeleteCriticalSection(&m_s);
	}
	void TinySpinLock::Lock()
	{
		EnterCriticalSection(&m_s);
	}
	BOOL TinySpinLock::TryLock()
	{
		return TryEnterCriticalSection(&m_s);
	}
	void TinySpinLock::Unlock()
	{
		LeaveCriticalSection(&m_s);
	}
	//////////////////////////////////////////////////////////////////////////
	TinySRWLock::TinySRWLock()
	{
		InitializeSRWLock(&m_SRW);
	}
	TinySRWLock::~TinySRWLock()
	{

	}
	TinySRWLock::operator SRWLOCK&()
	{
		return m_SRW;
	}
	void TinySRWLock::Lock(BOOL write)
	{
		if (write)
			AcquireSRWLockExclusive(&m_SRW);
		else
			AcquireSRWLockShared(&m_SRW);
	}
	void TinySRWLock::Unlock(BOOL write)
	{
		if (write)
			ReleaseSRWLockExclusive(&m_SRW);
		else
			ReleaseSRWLockShared(&m_SRW);
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
	TinyPerformanceLock::TinyPerformanceLock()
		:m_lock(NULL)
	{
		GetSystemInfo(&m_si);
	}
	TinyPerformanceLock::~TinyPerformanceLock()
	{

	}
	void TinyPerformanceLock::Lock(LONG value, UINT spin)
	{
		for (;;)
		{
			if (*m_lock == 0 && InterlockedCompareExchange(m_lock, 0, value))
			{
				return;
			}
			if (m_si.dwNumberOfProcessors > 1)
			{
				for (UINT s = 1; s < spin; s <<= 1)
				{
					for (UINT v = 0; v < s; v++)
					{
						__asm { pause };
					}
					if (*m_lock == 0 && InterlockedCompareExchange(m_lock, 0, value))
					{
						return;
					}
				}
			}
			SwitchToThread();
		}
	}
	void TinyPerformanceLock::Unlock()
	{
		*(m_lock) = 0;
	};
#endif // _WIN32
	//////////////////////////////////////////////////////////////////////////
	TinySemaphore::TinySemaphore()
		:m_hSemaphore(NULL)
	{
	}
	TinySemaphore::~TinySemaphore()
	{

	}
	BOOL TinySemaphore::Create(LONG lInitialCount, LONG lMaxCount, LPCTSTR pstrName, LPSECURITY_ATTRIBUTES lpsaAttributes)
	{
		m_hSemaphore = ::CreateSemaphore(lpsaAttributes, lInitialCount, lMaxCount, pstrName);
		return m_hSemaphore != NULL;
	}
	BOOL TinySemaphore::Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		m_hSemaphore = ::OpenSemaphore(dwDesiredAccess, bInheritHandle, lpName);
		return m_hSemaphore != NULL;
	}
	TinySemaphore::operator HANDLE() const
	{
		return m_hSemaphore;
	}
	HANDLE TinySemaphore::Handle() const
	{
		return m_hSemaphore;
	}
	BOOL TinySemaphore::Lock(DWORD dwTimeout)
	{
		ASSERT(m_hSemaphore != NULL);
		DWORD dwRet = ::WaitForSingleObject(m_hSemaphore, dwTimeout);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
			return TRUE;
		else
			return FALSE;
	}
	BOOL TinySemaphore::Unlock(LONG lCount, LPLONG lprevCount)
	{
		return ::ReleaseSemaphore(m_hSemaphore, lCount, lprevCount);
	}
	//////////////////////////////////////////////////////////////////////////
	TinyConditionVariable::TinyConditionVariable(CRITICAL_SECTION& cs)
		:m_cs(cs),
		m_allowSRW(FALSE)
	{
		InitializeConditionVariable(&m_cv);
	}
	TinyConditionVariable::TinyConditionVariable(SRWLOCK& lock)
		: m_lock(lock),
		m_allowSRW(TRUE)
	{
		InitializeConditionVariable(&m_cv);
	}
	TinyConditionVariable::~TinyConditionVariable()
	{

	}
	BOOL TinyConditionVariable::Lock(DWORD dwMS)
	{
		if (m_allowSRW)
		{
			return SleepConditionVariableSRW(&m_cv, &m_lock, dwMS, 0);
		}
		else
		{
			return SleepConditionVariableCS(&m_cv, &m_cs, dwMS);
		}
	}
	void TinyConditionVariable::Unlock(BOOL bAll)
	{
		if (bAll)
		{
			WakeAllConditionVariable(&m_cv);
		}
		else
		{
			WakeConditionVariable(&m_cv);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyWait::TinyWait()
		:m_hWaitHandle(NULL)
	{

	}
	TinyWait::~TinyWait()
	{

	}
	BOOL TinyWait::IsValid() const
	{
		return m_hWaitHandle != NULL;
	}
	BOOL TinyWait::RegisterOnce(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback)
	{
		return Register(handle, dwMS, WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE, std::move(callback));
	}
	BOOL TinyWait::Register(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback)
	{
		return Register(handle, dwMS, WT_EXECUTEINWAITTHREAD, std::move(callback));
	}
	BOOL TinyWait::Register(HANDLE handle, DWORD dwMS, DWORD dwFlag, Callback<void(BOOLEAN)>&& callback)
	{
		if (IsValid())
			return FALSE;
		if (RegisterWaitForSingleObject(&m_hWaitHandle, handle, &TinyWait::WaitOrTimerCallback, this, dwMS, dwFlag))
		{
			m_callback = std::move(callback);
			m_handle = handle;
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyWait::Unregister()
	{
		if (UnregisterWait(m_hWaitHandle))
		{
			m_hWaitHandle = NULL;
			return TRUE;
		}
		return FALSE;
	}
	void CALLBACK TinyWait::WaitOrTimerCallback(PVOID pThis, BOOLEAN b)
	{
		TinyWait* ps = reinterpret_cast<TinyWait*>(pThis);
		if (ps)
		{
			ps->m_callback(b);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyScopedLibrary::TinyScopedLibrary()
		: m_hInstance(NULL)
	{

	}
	TinyScopedLibrary::TinyScopedLibrary(LPCSTR pzName)
	{
		m_hInstance = ::LoadLibrary(pzName);
	}
	TinyScopedLibrary::~TinyScopedLibrary()
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
	}
	TinyScopedLibrary::operator HINSTANCE() const
	{
		return m_hInstance;
	}

	HINSTANCE TinyScopedLibrary::Handle() const
	{
		return m_hInstance;
	}
	FARPROC TinyScopedLibrary::GetFunctionPointer(LPCSTR lpProcName) const
	{
		return ::GetProcAddress(m_hInstance, lpProcName);
	}
	BOOL TinyScopedLibrary::IsValid() const
	{
		return m_hInstance != NULL;
	}
	void TinyScopedLibrary::Reset(HINSTANCE hInstance)
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
		m_hInstance = hInstance;
	}
	void TinyScopedLibrary::Reset(LPCSTR pzName)
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
		m_hInstance = LoadLibrary(pzName);
	}
};
