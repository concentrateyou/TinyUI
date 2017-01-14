#include "stdafx.h"
#include "DX.h"
#include "DX9Capture.h"
#include "DXGICapture.h"

namespace DXCapture
{
	DXGI_FORMAT GetDX10PlusTextureFormat(DXGI_FORMAT s)
	{
		switch (s)
		{
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		return s;
	}
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam)
	{
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(g_dx.m_hhk, code, wParam, lParam);
		SharedCaptureDATA* sharedCapture = g_dx.GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		if (sharedCapture->HwndCapture == reinterpret_cast<HWND>(wParam))
		{
			g_dx9.Reset();
			g_dxgi.Release();
			g_dx.m_exit.SetEvent();
		}
		return CallNextHookEx(g_dx.m_hhk, code, wParam, lParam);
	}
	//////////////////////////////////////////////////////////////////////////
	DX::DX()
		:m_hhk(NULL)
	{
	}

	DX::~DX()
	{
	}
	BOOL DX::Initialize()
	{
		if (!BuildEvents())
			return FALSE;
		if (!m_captureMemery.Open(SHAREDCAPTURE_MEMORY) &&
			!m_captureMemery.Create(SHAREDCAPTURE_MEMORY, sizeof(SharedCaptureDATA)))
			return FALSE;
		if (!m_captureMemery.Map(0, 0))
			return FALSE;
		if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE) &&
			!m_textureMemery.Create(TEXTURE_MEMORY, sizeof(SharedTextureDATA)))
			return FALSE;
		if (!m_textureMemery.Map(0, 0))
			return FALSE;
		return TRUE;
	}
	void DX::Uninitialize()
	{
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
		m_captureMemery.Unmap();
		m_captureMemery.Close();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
	}
	SharedCaptureDATA* DX::GetSharedCaptureDATA()
	{
		if (!m_captureMemery.Address())
		{
			if (!m_captureMemery.Open(SHAREDCAPTURE_MEMORY) &&
				!m_captureMemery.Create(SHAREDCAPTURE_MEMORY, sizeof(SharedCaptureDATA)))
				return NULL;
			if (!m_captureMemery.Map(0, 0))
				return NULL;
		}
		return reinterpret_cast<SharedCaptureDATA*>(m_captureMemery.Address());
	}
	SharedTextureDATA* DX::GetSharedTextureDATA()
	{
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE) &&
				!m_textureMemery.Create(TEXTURE_MEMORY, sizeof(SharedTextureDATA)))
				return NULL;
			if (!m_textureMemery.Map(0, 0))
				return NULL;
		}
		return reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
	}
	BOOL DX::BuildEvents()
	{
		DWORD dwProcessID = GetCurrentProcessId();
		if (!m_start && !m_start.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_stop && !m_stop.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", END_CAPTURE_EVENT, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_ready && !m_ready.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", CAPTURE_READY_EVENT, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_exit && !m_exit.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", CAPTURE_EXIT_EVENT, dwProcessID).c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX::SetWindowsHook()
	{
		UnhookWindowsHook();
		m_hhk = SetWindowsHookEx(WH_CBT, DXCbtFilter, NULL, GetCurrentThreadId());
		return m_hhk != NULL;
	}
	void DX::UnhookWindowsHook()
	{
		if (m_hhk != NULL)
		{
			UnhookWindowsHookEx(m_hhk);
			m_hhk = NULL;
		}
	}
}