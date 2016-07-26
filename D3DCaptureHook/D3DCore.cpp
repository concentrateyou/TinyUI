#include "stdafx.h"
#include "D3DCore.h"

CCriticalSection::CCriticalSection() throw()
{
	memset(&section, 0, sizeof(CRITICAL_SECTION));
}
CCriticalSection::~CCriticalSection()
{
}
HRESULT CCriticalSection::Lock() throw()
{
	EnterCriticalSection(&section);
	return S_OK;
}
BOOL CCriticalSection::Try() throw()
{
	return ::TryEnterCriticalSection(&section);
}
HRESULT CCriticalSection::Unlock() throw()
{
	LeaveCriticalSection(&section);
	return S_OK;
}
HRESULT CCriticalSection::Initialize() throw()
{
	InitializeCriticalSection(&section);
	return S_OK;
}
HRESULT CCriticalSection::Uninitialize() throw()
{
	DeleteCriticalSection(&section);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////
CEvent::CEvent()
	:m_hEvent(NULL)
{

}
CEvent::~CEvent()
{
	if (m_hEvent != NULL)
	{
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}
CEvent::operator HANDLE() const
{
	return (HANDLE)(this == NULL ? NULL : m_hEvent);
}
HANDLE CEvent::Handle()
{
	return m_hEvent;
}
BOOL CEvent::CreateEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR lpszNAme, LPSECURITY_ATTRIBUTES lpsaAttribute)
{
	m_hEvent = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, lpszNAme);
	return m_hEvent == NULL ? FALSE : TRUE;
}
BOOL CEvent::OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
{
	m_hEvent = ::OpenEvent(dwDesiredAccess, bInheritHandle, lpName);
	return m_hEvent == NULL ? FALSE : TRUE;
}
BOOL CEvent::SetEvent()
{
	ASSERT(m_hEvent != NULL);
	return ::SetEvent(m_hEvent);
}
BOOL CEvent::PulseEvent()
{
	ASSERT(m_hEvent != NULL);
	return ::PulseEvent(m_hEvent);
}
BOOL CEvent::ResetEvent()
{
	ASSERT(m_hEvent != NULL);
	return ::ResetEvent(m_hEvent);
}
BOOL CEvent::Lock(DWORD dwTimeout)
{
	ASSERT(m_hEvent != NULL);
	DWORD dwRet = ::WaitForSingleObject(m_hEvent, dwTimeout);
	if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
		return TRUE;
	else
		return FALSE;
}
BOOL CEvent::Unlock()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////
CMutex::CMutex()
	:m_hMutex(NULL)
{

}
CMutex::~CMutex()
{
	if (m_hMutex != NULL)
	{
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}
BOOL CMutex::Create(BOOL bInitiallyOwn, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpsaAttribute)
{
	m_hMutex = ::CreateMutex(lpsaAttribute, bInitiallyOwn, lpszName);
	return m_hMutex == NULL ? FALSE : TRUE;
}
BOOL CMutex::Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
{
	m_hMutex = ::OpenMutex(dwDesiredAccess, bInheritHandle, lpName);
	return m_hMutex == NULL ? FALSE : TRUE;
}
CMutex::operator HANDLE() const
{
	return (HANDLE)(this == NULL ? NULL : m_hMutex);
}
HANDLE CMutex::Handle()
{
	return m_hMutex;
}
BOOL CMutex::Lock(DWORD dwTimeout)
{
	ASSERT(m_hMutex != NULL);
	DWORD dwRet = ::WaitForSingleObject(m_hMutex, dwTimeout);
	if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
		return TRUE;
	else
		return FALSE;
}
BOOL CMutex::Unlock()
{
	ASSERT(m_hMutex != NULL);
	return ::ReleaseMutex(m_hMutex);
}
//////////////////////////////////////////////////////////////////////////
CLock::CLock()
{
	m_section.Initialize();
}
CLock::~CLock()
{
	m_section.Uninitialize();
}
void CLock::Acquire()
{
	m_section.Lock();
}
void CLock::Release()
{
	m_section.Unlock();
}
BOOL CLock::Try()
{
	return m_section.Try();
}