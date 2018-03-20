#pragma once
#include "TinyMsg.h"
#include "TinyCore.h"
#include "TinyHandleMap.h"
#include "TinyCollection.h"

namespace TinyUI
{
	class TinyHandleHWND;
	class TinyHandleHMENU;
	class TinyHandleHDC;
	class TinyHandleHBITMAP;
	class TinyHandleHFONT;
	class TinyHandleHPEN;
	class TinyHandleHBRUSH;
	class TinyHandleHPALETTE;
	class TinyHandleHRGN;
	/// <summary>
	/// TinyUI的应用实例
	/// </summary>
	class TinyApplication
	{
	private:
		TinyApplication();
	protected:
		~TinyApplication();
	public:
		//单例模式
		static TinyApplication* GetInstance() throw();
		//分静态成员
		HINSTANCE Handle() throw();
		BOOL AddMessageLoop(TinyMessageLoop* pMsgLoop);
		BOOL RemoveMessageLoop();
		TinyMessageLoop* GetMessageLoop(DWORD dwThreadID = ::GetCurrentThreadId());
		TinyTimerQueue&	 GetTimers();
		BOOL Initialize(HINSTANCE m_hInstance, LPTSTR m_lpCmdLine, INT m_nCmdShow, LPCTSTR lpTableName);
		BOOL Uninitialize();
	private:
		INT												m_iCmdShow;
		HINSTANCE										m_hInstance;
		LPTSTR											m_hCmdLine;
		HACCEL											m_hAccTable;
		ULONG_PTR										m_token;
		WSADATA											m_wsd;
		TinyLock										m_lock;
		TinyTimerQueue									m_timers;
		TinySimpleMap<DWORD, TinyMessageLoop*>			m_msgLoops;
	};
	//////////////////////////////////////////////////////////////////////////
	LONG WINAPI TOP_LEVEL_EXCEPTION_FILTER(struct _EXCEPTION_POINTERS *pExp);
}


