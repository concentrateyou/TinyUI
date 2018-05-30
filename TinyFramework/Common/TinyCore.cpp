#include "../stdafx.h"
#include "TinyCore.h"
#include "TinyLogging.h"

namespace TinyFramework
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
	BOOL TinySRWLock::Try(BOOL bExclusive)
	{
		BOOL bRes = FALSE;
		if (bExclusive)
		{
			bRes = TryAcquireSRWLockExclusive(&m_SRW);
		}
		else
		{
			bRes = TryAcquireSRWLockShared(&m_SRW);
		}
		return bRes;
	}
	void TinySRWLock::Lock(BOOL bExclusive)
	{
		if (bExclusive)
		{
			AcquireSRWLockExclusive(&m_SRW);
		}
		else
		{
			AcquireSRWLockShared(&m_SRW);
		}
	}
	void TinySRWLock::Unlock(BOOL bExclusive)
	{
		if (bExclusive)
		{
			ReleaseSRWLockExclusive(&m_SRW);
		}
		else
		{
			ReleaseSRWLockShared(&m_SRW);
		}
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
						//__asm { pause };
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
	TinyConditionVariable::TinyConditionVariable()
	{
		InitializeConditionVariable(&m_cv);
	}
	TinyConditionVariable::~TinyConditionVariable()
	{

	}
	BOOL TinyConditionVariable::Lock(CRITICAL_SECTION& cs, DWORD dwMS)
	{
		return SleepConditionVariableCS(&m_cv, &cs, dwMS);
	}
	BOOL TinyConditionVariable::Lock(SRWLOCK& lock, DWORD dwMS)
	{
		return SleepConditionVariableSRW(&m_cv, &lock, dwMS, 0);
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
	TinyWaiter::TinyWaiter()
		:m_hWaitHandle(NULL)
	{

	}
	TinyWaiter::~TinyWaiter()
	{

	}
	BOOL TinyWaiter::IsEmpty() const
	{
		return m_hWaitHandle == NULL;
	}
	BOOL TinyWaiter::RegisterOnce(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback)
	{
		return Register(handle, dwMS, WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE, std::move(callback));
	}
	BOOL TinyWaiter::Register(HANDLE handle, DWORD dwMS, Callback<void(BOOLEAN)>&& callback)
	{
		return Register(handle, dwMS, WT_EXECUTEINWAITTHREAD, std::move(callback));
	}
	BOOL TinyWaiter::Register(HANDLE handle, DWORD dwMS, DWORD dwFlag, Callback<void(BOOLEAN)>&& callback)
	{
		if (IsEmpty())
			return FALSE;
		if (RegisterWaitForSingleObject(&m_hWaitHandle, handle, &TinyWaiter::WaitOrTimerCallback, this, dwMS, dwFlag))
		{
			m_callback = std::move(callback);
			m_handle = handle;
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyWaiter::Unregister()
	{
		BOOL bRes = UnregisterWait(m_hWaitHandle);
		m_hWaitHandle = NULL;
		return bRes;
	}
	void CALLBACK TinyWaiter::WaitOrTimerCallback(PVOID pThis, BOOLEAN b)
	{
		TinyWaiter* ps = reinterpret_cast<TinyWaiter*>(pThis);
		if (ps)
		{
			ps->m_callback(b);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyTimerQueue::TinyTimerQueue()
		:m_handle(NULL)
	{

	}
	TinyTimerQueue::~TinyTimerQueue()
	{

	}
	BOOL TinyTimerQueue::Create()
	{
		m_handle = CreateTimerQueue();
		if (!m_handle)
			return FALSE;
		return TRUE;
	}
	HANDLE TinyTimerQueue::Register(WAITORTIMERCALLBACK callback, LPVOID ps, WORD dueTime, DWORD period, DWORD dwFlag)
	{
		if (!m_handle)
			return FALSE;
		HANDLE hTimer = NULL;
		if (CreateTimerQueueTimer(&hTimer, m_handle, callback, ps, dueTime, period, dwFlag))
			return hTimer;
		return NULL;
	}
	BOOL TinyTimerQueue::Change(HANDLE hTimer, WORD dueTime, DWORD period)
	{
		if (!m_handle || !hTimer)
			return FALSE;
		return ChangeTimerQueueTimer(m_handle, hTimer, dueTime, period);
	}
	BOOL TinyTimerQueue::Unregister(HANDLE hTimer)
	{
		if (!m_handle || !hTimer)
			return FALSE;
		return DeleteTimerQueueTimer(m_handle, hTimer, NULL);
	}
	void TinyTimerQueue::Destory()
	{
		if (m_handle != NULL)
		{
			DeleteTimerQueue(m_handle);
			m_handle = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyPerformanceTimer::TinyPerformanceTimer()
		:m_timerID(NULL)
	{
		m_event.CreateEvent();
	}
	BOOL TinyPerformanceTimer::SetCallback(INT delay, Closure&& callback)
	{
		if (delay <= 0)
			return TRUE;
		m_callback = std::move(callback);
		CloseTimer();
		m_timerID = timeSetEvent(delay, 1, &TinyPerformanceTimer::TimerCallback, reinterpret_cast<DWORD_PTR>(this), TIME_PERIODIC);
		return m_timerID != NULL;
	}
	BOOL TinyPerformanceTimer::Waiting(INT delay, DWORD dwMilliseconds)
	{
		if (delay <= 0)
			return TRUE;
		CloseTimer();
		m_timerID = timeSetEvent(delay, 1, reinterpret_cast<LPTIMECALLBACK>(m_event.Handle()), NULL, TIME_CALLBACK_EVENT_SET);
		if (m_timerID != NULL)
		{
			DWORD dwRes = WaitForSingleObject(m_event, dwMilliseconds);
			if (dwRes == WAIT_OBJECT_0 || dwRes == WAIT_ABANDONED)
				return TRUE;
		}
		return FALSE;
	}
	void TinyPerformanceTimer::Close()
	{
		m_event.Close();
		CloseTimer();
	}
	TinyPerformanceTimer::~TinyPerformanceTimer()
	{
		Close();
	}
	void TinyPerformanceTimer::CloseTimer()
	{
		if (m_timerID != NULL)
		{
			timeKillEvent(m_timerID);
			m_timerID = NULL;
		}
	}
	void CALLBACK TinyPerformanceTimer::TimerCallback(UINT uTimerID, UINT  uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
	{
		TinyPerformanceTimer* pThis = reinterpret_cast<TinyPerformanceTimer*>(dwUser);
		if (pThis != NULL && !pThis->m_callback.IsNull())
		{
			pThis->m_callback();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyWaitableTimer::TinyWaitableTimer()
		:m_handle(NULL),
		m_bBreak(FALSE)
	{

	}
	TinyWaitableTimer::~TinyWaitableTimer()
	{

	}
	BOOL TinyWaitableTimer::IsEmpty() const
	{
		return m_handle == NULL;
	}
	BOOL TinyWaitableTimer::Create(BOOL bManualReset, LPCSTR pszName)
	{
		Close();
		m_handle = CreateWaitableTimer(NULL, bManualReset, pszName);
		return m_handle != NULL;
	}
	BOOL TinyWaitableTimer::Open(LPCSTR pszName)
	{
		Close();
		m_handle = OpenWaitableTimer(EVENT_ALL_ACCESS, TRUE, pszName);
		return m_handle != NULL;
	}
	BOOL TinyWaitableTimer::SetCallback(LONG due, Closure&& callback)
	{
		m_callback = std::move(callback);
		if (!m_handle)
			return FALSE;
		LARGE_INTEGER lElapse;
		lElapse.QuadPart = -((INT)(due) * 10000);
		return SetWaitableTimer(m_handle, &lElapse, 0, TinyWaitableTimer::TimerCallback, this, FALSE);
	}
	BOOL TinyWaitableTimer::SetWaiting(LONG due)
	{
		if (!m_handle)
			return FALSE;
		LARGE_INTEGER lElapse;
		lElapse.QuadPart = -((INT)(due) * 10000);
		return SetWaitableTimer(m_handle, &lElapse, 0, NULL, this, FALSE);
	}
	BOOL TinyWaitableTimer::Waiting()
	{
		if (!m_handle)
			return FALSE;
		return (WaitForSingleObjectEx(m_handle, INFINITE, TRUE) == WAIT_OBJECT_0);
	}
	BOOL TinyWaitableTimer::Cancel()
	{
		if (!m_handle)
			return FALSE;
		return CancelWaitableTimer(m_handle);
	}
	void TinyWaitableTimer::Close()
	{
		if (m_handle != NULL)
		{
			CancelWaitableTimer(m_handle);
			LARGE_INTEGER lElapse;
			lElapse.QuadPart = 0;
			SetWaitableTimer(m_handle, &lElapse, 0, TinyWaitableTimer::TimerCallback, this, FALSE);
			CloseHandle(m_handle);
			m_handle = NULL;
		}
	}
	void CALLBACK TinyWaitableTimer::TimerCallback(LPVOID lpArgToCompletionRoutine, DWORD  dwTimerLowValue, DWORD  dwTimerHighValue)
	{
		TinyWaitableTimer* pThis = static_cast<TinyWaitableTimer*>(lpArgToCompletionRoutine);
		if (pThis != NULL && !pThis->m_callback.IsNull())
		{
			pThis->m_callback();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinySimpleMap<UINT_PTR, TinyTimer*> TinyTimer::m_map;
	TinyTimer::TinyTimer()
		:m_timerID(0)
	{

	}
	TinyTimer::~TinyTimer()
	{

	}
	BOOL TinyTimer::SetCallback(UINT elapse, Closure&& callback)
	{
		Close();
		m_callback = std::move(callback);
		m_timerID = ::SetTimer(NULL, 0, elapse, TinyTimer::TimerCallback);
		if (m_timerID != 0)
		{
			return m_map.Add(m_timerID, this);
		}
		return FALSE;
	}
	void TinyTimer::Close()
	{
		if (m_timerID != 0)
		{
			KillTimer(NULL, m_timerID);
			m_timerID = 0;
		}
		m_map.Remove(m_timerID);
	}
	void CALLBACK TinyTimer::TimerCallback(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD  dwTime)
	{
		TinyTimer* pThis = *(TinyTimer::m_map.Find(idEvent));
		if (pThis != NULL && !pThis->m_callback.IsNull())
		{
			pThis->m_callback();
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
	//////////////////////////////////////////////////////////////////////////
	void WINAPI SleepMS(DWORD dwMS)
	{
		static auto NtDll = GetModuleHandleW(L"ntdll.dll");
		static auto NtDelayExecution = (long(__stdcall*)(BOOL alertable, PLARGE_INTEGER delayInterval))GetProcAddress(NtDll, "NtDelayExecution");
		static auto ZwSetTimerResolution = (long(__stdcall*)(ULONG requestedRes, BOOL setNew, ULONG* actualRes))GetProcAddress(NtDll, "ZwSetTimerResolution");
		static BOOL resolutionSet = FALSE;
		if (!resolutionSet)
		{
			ULONG actualResolution;
			ZwSetTimerResolution(1, TRUE, &actualResolution);
			resolutionSet = TRUE;
		}
		LARGE_INTEGER interval;
		interval.QuadPart = -1 * (LONGLONG)(dwMS * 10000);
		NtDelayExecution(FALSE, &interval);
	}
};
