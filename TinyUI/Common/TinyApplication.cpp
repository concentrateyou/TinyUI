#include "../stdafx.h"
#include "TinyApplication.h"

namespace TinyUI
{
	TinyApplication::TinyApplication()
		:m_hInstance(NULL),
		m_hCmdLine(NULL),
		m_hAccTable(NULL),
		m_iCmdShow(-1),
		m_token(0)
	{
	}
	TinyApplication* TinyApplication::GetInstance() throw()
	{
		static TinyApplication instance;
		return &instance;
	}
	HINSTANCE TinyApplication::Handle() throw()
	{
		return m_hInstance;
	}
	BOOL TinyApplication::AddMessageLoop(TinyMessageLoop* pMsgLoop)
	{
		TinySpinLock spin;
		if (!spin.Initialize())
		{
			TRACE(_T("ERROR : Unable to lock critical section in AddMessageLoop.\n"));
			ASSERT(FALSE);
			return FALSE;
		}
		spin.Lock();
		ASSERT(pMsgLoop != NULL);
		ASSERT(m_msgLoops.Lookup(::GetCurrentThreadId()) == NULL);
		BOOL bRet = m_msgLoops.Add(::GetCurrentThreadId(), pMsgLoop) != NULL;
		spin.Unlock();
		spin.Uninitialize();
		return bRet;
	}
	BOOL TinyApplication::RemoveMessageLoop()
	{
		TinySpinLock spin;
		if (!spin.Initialize())
		{
			TRACE(_T("ERROR : Unable to lock critical section in RemoveMessageLoop.\n"));
			ASSERT(FALSE);
			return FALSE;
		}
		spin.Lock();
		BOOL bRet = m_msgLoops.Remove(::GetCurrentThreadId());
		spin.Unlock();
		spin.Uninitialize();
		return bRet;
	}
	TinyMessageLoop* TinyApplication::GetMessageLoop(DWORD dwThreadID)
	{
		TinySpinLock spin;
		if (!spin.Initialize())
		{
			TRACE(_T("ERROR : Unable to lock critical section in SysMessageLoop::GetMessageLoop.\n"));
			ASSERT(FALSE);
			return NULL;
		}
		spin.Lock();
		TinyMessageLoop* ps = *m_msgLoops.Lookup(dwThreadID);
		spin.Unlock();
		spin.Uninitialize();
		return ps;
	}
	BOOL TinyApplication::Initialize(HINSTANCE m_hInstance, LPTSTR m_lpCmdLine, INT m_nCmdShow, LPCTSTR lpTableName)
	{
		if (FAILED(OleInitialize(NULL)))
			return FALSE;
		if (WSAStartup(MAKEWORD(2, 2), &m_wsd) != NOERROR)
			return FALSE;
		this->m_hCmdLine = m_lpCmdLine;
		this->m_hInstance = m_hInstance;
		this->m_iCmdShow = m_nCmdShow;
		this->m_hAccTable = LoadAccelerators(m_hInstance, lpTableName);
		if (!m_token)
		{
			GdiplusStartupInput input;
			GdiplusStartupOutput output;
			Status status = GdiplusStartup(&m_token, &input, &output);
			return (status == Ok);
		}
		return TRUE;
	}
	BOOL TinyApplication::Uninitialize()
	{
		if (WSACleanup() != NOERROR)
			return FALSE;
		m_hInstance = NULL;
		m_hCmdLine = NULL;
		if (m_token != 0)
		{
			GdiplusShutdown(m_token);
			m_token = 0;
		}
		OleUninitialize();
		return TRUE;
	}

	TinyHandleMap<HWND, TinyHandleHWND*>& TinyApplication::GetMapHWND()
	{
		return m_MapHWND;
	}
	TinyHandleMap<HMENU, TinyHandleHMENU*>& TinyApplication::GetMapHMENU()
	{
		return m_MapHMENU;
	}
	TinyHandleMap<HDC, TinyHandleHDC*>&	TinyApplication::GetMapHDC()
	{
		return m_MapHDC;
	}
	TinyHandleMap<HBITMAP, TinyHandleHBITMAP*>&	TinyApplication::GetMapHBITMAP()
	{
		return m_MapHBITMAP;
	}
	TinyHandleMap<HFONT, TinyHandleHFONT*>&	TinyApplication::GetMapHFONT()
	{
		return m_MapHFONT;
	}
	TinyHandleMap<HPEN, TinyHandleHPEN*>& TinyApplication::GetMapHPEN()
	{
		return m_MapHPEN;
	}
	TinyHandleMap<HBRUSH, TinyHandleHBRUSH*>& TinyApplication::GetMapHBRUSH()
	{
		return m_MapHBRUSH;
	}
	TinyHandleMap<HPALETTE, TinyHandleHPALETTE*>& TinyApplication::GetMapHPALETTE()
	{
		return m_MapHPALETTE;
	}
	TinyHandleMap<HRGN, TinyHandleHRGN*>& TinyApplication::GetMapHRGN()
	{
		return m_MapHRGN;
	}
	TinyHandleMap<HIMAGELIST, TinyHandleHIMAGELIST*>& TinyApplication::GetMapHIMAGELIST()
	{
		return m_MapHIMAGELIST;
	}
	TinyApplication::~TinyApplication()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	LONG WINAPI TOP_LEVEL_EXCEPTION_FILTER(struct _EXCEPTION_POINTERS *pExp)
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
}
