#include "../stdafx.h"
#include "TinyMsg.h"

namespace TinyFramework
{
	TinyMsg::TinyMsg() : cbSize(sizeof(TinyMsg)), bHandled(TRUE)
	{
		hwnd = NULL;
		message = 0;
		wParam = 0;
		lParam = 0;
		time = 0;
		pt.x = pt.y = 0;
	}
	TinyMsg::TinyMsg(HWND hWnd, UINT uMsg, WPARAM wParamIn, LPARAM lParamIn, DWORD dwTime, POINT ptIn, BOOL bHandledIn) : cbSize(sizeof(TinyMsg)), bHandled(bHandledIn)
	{
		hwnd = hWnd;
		message = uMsg;
		wParam = wParamIn;
		lParam = lParamIn;
		time = dwTime;
		pt = ptIn;
	}
	TinyMsg::TinyMsg(HWND hWnd, UINT uMsg, WPARAM wParamIn, LPARAM lParamIn, BOOL bHandledIn) : cbSize(sizeof(TinyMsg)), bHandled(bHandledIn)
	{
		hwnd = hWnd;
		message = uMsg;
		wParam = wParamIn;
		lParam = lParamIn;
		time = 0;
		pt.x = pt.y = 0;
	}
	TinyMsg::TinyMsg(MSG& msg, BOOL bHandledIn) : cbSize(sizeof(TinyMsg)), bHandled(bHandledIn)
	{
		hwnd = msg.hwnd;
		message = msg.message;
		wParam = msg.wParam;
		lParam = msg.lParam;
		time = msg.time;
		pt = msg.pt;
	}
	/////////////////////////////////////////////////////////////////////////
	TinyMessageFilters::TinyMessageFilters()
	{

	}
	TinyMessageFilters::~TinyMessageFilters()
	{
	}
	BOOL TinyMessageFilters::Add(TinyMessageFilter*  element)
	{
		return m_array.Add(element);
	}
	BOOL TinyMessageFilters::Remove(TinyMessageFilter* element)
	{
		return m_array.Remove(element);
	}
	BOOL TinyMessageFilters::RemoveAt(INT index)
	{
		return m_array.RemoveAt(index);
	}
	void TinyMessageFilters::RemoveAll()
	{
		m_array.RemoveAll();
	}
	TinyMessageFilter* TinyMessageFilters::operator[](INT index)
	{
		return m_array[index];
	}
	const TinyMessageFilter* TinyMessageFilters::operator[](INT index) const
	{
		return m_array[index];
	}
	INT TinyMessageFilters::Lookup(TinyMessageFilter* element) const
	{
		return m_array.Lookup(element);
	}
	INT  TinyMessageFilters::GetSize() const
	{
		return m_array.GetSize();
	}
	//////////////////////////////////////////////////////////////////////////
	TinyIdleHandlers::TinyIdleHandlers()
	{

	}
	TinyIdleHandlers::~TinyIdleHandlers()
	{
	}
	BOOL TinyIdleHandlers::Add(TinyIdleHandler*  element)
	{
		return m_array.Add(element);
	}
	BOOL TinyIdleHandlers::Remove(TinyIdleHandler* element)
	{
		return m_array.Remove(element);
	}
	BOOL TinyIdleHandlers::RemoveAt(INT index)
	{
		return m_array.RemoveAt(index);
	}
	void TinyIdleHandlers::RemoveAll()
	{
		m_array.RemoveAll();
	}
	TinyIdleHandler* TinyIdleHandlers::operator[](INT index)
	{
		return m_array[index];
	}
	const TinyIdleHandler* TinyIdleHandlers::operator[](INT index) const
	{
		return m_array[index];
	}
	INT TinyIdleHandlers::Lookup(TinyIdleHandler* element) const
	{
		return m_array.Lookup(element);
	}
	INT  TinyIdleHandlers::GetSize() const
	{
		return m_array.GetSize();
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL TinyMessageLoop::AddMessageFilter(TinyMessageFilter* pMessageFilter)
	{
		return m_messageFilters.Add(pMessageFilter);
	}
	BOOL TinyMessageLoop::RemoveMessageFilter(TinyMessageFilter* pMessageFilter)
	{
		return m_messageFilters.Remove(pMessageFilter);
	}
	BOOL TinyMessageLoop::AddIdleHandler(TinyIdleHandler* pIdleHandler)
	{
		return m_idleHandles.Add(pIdleHandler);
	}
	BOOL TinyMessageLoop::RemoveIdleHandler(TinyIdleHandler* pIdleHandler)
	{
		return m_idleHandles.Remove(pIdleHandler);
	}
#define WM_SYSTIMER 0x0118
	static BOOL IsIdleMessage(MSG* pMsg)
	{
		switch (pMsg->message)
		{
		case WM_MOUSEMOVE:
#ifndef _WIN32_WCE
		case WM_NCMOUSEMOVE:
#endif 
		case WM_PAINT:
		case WM_SYSTIMER:
			return FALSE;
		}
		return TRUE;
	}

	INT TinyMessageLoop::MessageLoop()
	{
		BOOL bRes;
		BOOL bIdle = TRUE;
		INT	 idleCount = 0;
		for (;;)
		{
			while (bIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!OnIdle(idleCount++))
				{
					bIdle = FALSE;
				}
			}
			bRes = ::GetMessage(&m_msg, NULL, 0, 0);
			if (bRes == -1)
			{
				TRACE(TEXT("::GetMessage returned -1 (error)\n"));
				continue;
			}
			else if (!bRes)
			{
				TRACE(TEXT("TinyMessageLoop::Run - exiting\n"));
				break;
			}
			if (!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if (IsIdleMessage(&m_msg))
			{
				bIdle = TRUE;
				idleCount = 0;
			}
		}
		return (INT)m_msg.wParam;
	}


	BOOL TinyMessageLoop::PreTranslateMessage(MSG* pMsg)
	{
		for (INT i = m_messageFilters.GetSize() - 1; i >= 0; i--)
		{
			TinyMessageFilter* pMessageFilter = m_messageFilters[i];
			if (pMessageFilter != NULL &&
				pMessageFilter->PreTranslateMessage(pMsg))
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL TinyMessageLoop::OnIdle(INT /*idleCount*/)
	{
		for (INT i = 0; i < m_idleHandles.GetSize(); i++)
		{
			TinyIdleHandler* pIdleHandler = m_idleHandles[i];
			if (pIdleHandler != NULL)
			{
				pIdleHandler->OnIdle();
			}
		}
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
}
