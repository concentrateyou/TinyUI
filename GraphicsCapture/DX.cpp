#include "stdafx.h"
#include "DX.h"
#include "DX8GraphicsCapture.h"
#include "DX9GraphicsCapture.h"
#include "DXGIGraphicsCapture.h"

namespace GraphicsCapture
{
	DXGI_FORMAT GetDX10PlusTextureFormat(DXGI_FORMAT s)
	{
		switch (s)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		}
		return s;
	}
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam)
	{
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(g_dx.m_hhk, code, wParam, lParam);
		HookDATA* sharedCapture = g_dx.GetHookDATA();
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
		{
			return FALSE;
		}
		if (!m_mutes[0].Open(MUTEX_ALL_ACCESS, FALSE, MUTEX_TEXTURE1) && !m_mutes[0].Create(FALSE, MUTEX_TEXTURE1, NULL))
		{
			return FALSE;
		}
		if (!m_mutes[1].Open(MUTEX_ALL_ACCESS, FALSE, MUTEX_TEXTURE2) && !m_mutes[1].Create(FALSE, MUTEX_TEXTURE2, NULL))
		{
			return FALSE;
		}
		if (!m_captureMemery.Open(SHAREDCAPTURE_MEMORY) && !m_captureMemery.Create(SHAREDCAPTURE_MEMORY, sizeof(HookDATA)))
		{
			return FALSE;
		}
		if (!m_captureMemery.Map(0, 0))
		{
			return FALSE;
		}
		return TRUE;
	}
	void DX::Uninitialize()
	{
		m_mutes[0].Close();
		m_mutes[1].Close();
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
		m_captureMemery.Unmap();
		m_captureMemery.Close();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
	}
	HookDATA* DX::GetHookDATA()
	{
		if (!m_captureMemery.Address())
		{
			if (!m_captureMemery.Open(SHAREDCAPTURE_MEMORY) && !m_captureMemery.Create(SHAREDCAPTURE_MEMORY, sizeof(HookDATA)))
			{
				return NULL;
			}
			if (!m_captureMemery.Map(0, 0))
			{
				return NULL;
			}
		}
		return reinterpret_cast<HookDATA*>(m_captureMemery.Address());
	}
	SharedTextureDATA* DX::GetSharedTextureDATA(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE) && !m_textureMemery.Create(TEXTURE_MEMORY, dwSize))
			{
				return NULL;
			}
			if (!m_textureMemery.Map(0, dwSize))
			{
				return NULL;
			}
		}
		return reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
	}
	BYTE* DX::GetSharedTexture(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE) && !m_textureMemery.Create(TEXTURE_MEMORY, dwSize))
			{
				return NULL;
			}
			if (!m_textureMemery.Map(0, dwSize))
			{
				return NULL;
			}
		}
		return reinterpret_cast<BYTE*>(m_textureMemery.Address());
	}
	BOOL DX::BuildEvents()
	{
		DWORD dwProcessID = GetCurrentProcessId();
		if (!m_start.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_START, dwProcessID).c_str()))
		{
			return FALSE;
		}
		LOG(INFO) << "m_start CreateEvent:" << StringPrintf("%s%d", EVENT_CAPTURE_START, dwProcessID).c_str();
		if (!m_stop.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_STOP, dwProcessID).c_str()))
		{
			return FALSE;
		}
		LOG(INFO) << "m_stop CreateEvent:" << StringPrintf("%s%d", EVENT_CAPTURE_STOP, dwProcessID).c_str();
		if (!m_ready.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_HOOK_READY, dwProcessID).c_str()))
		{
			return FALSE;
		}
		LOG(INFO) << "m_ready CreateEvent:" << StringPrintf("%s%d", EVENT_HOOK_READY, dwProcessID).c_str();
		if (!m_exit.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_HOOK_EXIT, dwProcessID).c_str()))
		{
			return FALSE;
		}
		LOG(INFO) << "m_exit CreateEvent:" << StringPrintf("%s%d", EVENT_HOOK_EXIT, dwProcessID).c_str();
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