#include "stdafx.h"
#include "DXCommon.h"
#include "DX9Capture.h"
#include "DXGICapture.h"

namespace DXCapture
{
	DXGlobal::DXGlobal()
		:m_hhk(NULL)
	{

	}
	DXGlobal::~DXGlobal()
	{
		Unhook();
	}
	DXGlobal& DXGlobal::Instance()
	{
		static DXGlobal gl;
		return gl;
	}
	SharedCaptureDATA* DXGlobal::GetSharedCaptureDATA()
	{
		if (!m_memery.Address())
		{
			if (!m_memery.Open(SHAREDCAPTURE_MEMORY) &&
				!m_memery.Create(SHAREDCAPTURE_MEMORY, sizeof(SharedCaptureDATA)))
				return FALSE;
			if (!m_memery.Map(0, 0))
				return FALSE;
		}
		return (SharedCaptureDATA*)m_memery.Address();
	}
	SharedTextureDATA* DXGlobal::GetSharedTextureDATA()
	{
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE) &&
				!m_textureMemery.Create(TEXTURE_MEMORY, sizeof(SharedTextureDATA)))
				return FALSE;
			if (!m_textureMemery.Map(0, 0))
				return FALSE;
		}
		return (SharedTextureDATA*)m_textureMemery.Address();
	}
	BOOL DXGlobal::BuildEvents()
	{
		DWORD dwProcessID = GetCurrentProcessId();
		if (!m_start)
		{
			string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, dwProcessID);
			if (!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_stop)
		{
			string name = StringPrintf("%s%d", END_CAPTURE_EVENT, dwProcessID);
			if (!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_ready)
		{
			string name = StringPrintf("%s%d", CAPTURE_READY_EVENT, dwProcessID);
			if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_exit)
		{
			string name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, dwProcessID);
			if (!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		return TRUE;
	}
	BOOL DXGlobal::Hook()
	{
		if (m_hhk != NULL)
			UnhookWindowsHookEx(m_hhk);
		m_hhk = SetWindowsHookEx(WH_CBT, DXGlobal::CbtFilterHook, NULL, GetCurrentThreadId());
		return m_hhk != NULL;
	}
	void DXGlobal::Unhook()
	{
		if (m_hhk != NULL)
		{
			UnhookWindowsHookEx(m_hhk);
			m_hhk = NULL;
		}
	}
	void DXGlobal::Release()
	{
		m_textureMemery.Unmap();
		m_textureMemery.Close();
		m_memery.Unmap();
		m_memery.Close();
	}
	LRESULT CALLBACK DXGlobal::CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam)
	{
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(DXGlobal::Instance().m_hhk, code, wParam, lParam);
		SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		if (sharedCapture->HwndCapture == reinterpret_cast<HWND>(wParam))
		{
			DXGlobal::Instance().m_exit.SetEvent();
			DX9Capture::Instance().Reset(FALSE);
			DXGICapture::Instance().Reset(FALSE);
			DXGlobal::Instance().Release();
		}
		return CallNextHookEx(DXGlobal::Instance().m_hhk, code, wParam, lParam);
	}
}

