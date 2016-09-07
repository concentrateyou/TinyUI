#include "../stdafx.h"
#include "TinyCore.h"

namespace TinyUI
{
	/////////////////////////////////////////////////////////////////////////
	TinyCriticalSection::TinyCriticalSection() throw()
	{
		ZeroMemory(&section, sizeof(section));
		InitializeCriticalSection(&section);
	}
	TinyCriticalSection::~TinyCriticalSection()
	{
		DeleteCriticalSection(&section);
	}
	void TinyCriticalSection::Lock() throw()
	{
		EnterCriticalSection(&section);
	}
	BOOL TinyCriticalSection::TryLock() throw()
	{
		return ::TryEnterCriticalSection(&section);
	}
	void TinyCriticalSection::Unlock() throw()
	{
		LeaveCriticalSection(&section);
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
		return (HANDLE)(this == NULL ? NULL : m_hEvent);
	}
	HANDLE TinyEvent::Handle() const
	{
		return m_hEvent;
	}
	BOOL TinyEvent::CreateEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR lpszNAme, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hEvent = ::CreateEvent(lpsaAttribute, bManualReset,
			bInitiallyOwn, lpszNAme);
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
	//////////////////////////////////////////////////////////////////////////
	TinyLock::TinyLock()
	{

	}
	TinyLock::~TinyLock()
	{
	}
	void TinyLock::Lock()
	{
		m_section.Lock();
	}
	void TinyLock::Unlock()
	{
		m_section.Unlock();
	}
	BOOL TinyLock::TryLock()
	{
		return m_section.TryLock();
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
		ZeroMemory(&section, sizeof(section));
	}
	TinySpinLock::~TinySpinLock()
	{
		Uninitialize();
	}
	BOOL TinySpinLock::Initialize(DWORD dwSpinCount) throw()
	{
		return InitializeCriticalSectionAndSpinCount(&section, dwSpinCount);
	}
	void TinySpinLock::Uninitialize()throw()
	{
		DeleteCriticalSection(&section);
	}
	void TinySpinLock::Lock()
	{
		EnterCriticalSection(&section);
	}
	BOOL TinySpinLock::TryLock()
	{
		return TryEnterCriticalSection(&section);
	}
	void TinySpinLock::Unlock()
	{
		LeaveCriticalSection(&section);
	}
	//////////////////////////////////////////////////////////////////////////
	TinySRWLock::TinySRWLock()
	{
		InitializeSRWLock(&m_SRW);
	}
	TinySRWLock::~TinySRWLock()
	{

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
	TinyScopedLibrary::TinyScopedLibrary()
		:m_hInstance(NULL)
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
