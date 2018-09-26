#include "stdafx.h"
#include "DX.h"
#include "DX8GraphicsCapture.h"
#include "DX9GraphicsCapture.h"
#include "DXGIGraphicsCapture.h"

namespace GraphicsCapture
{
	DXGI_FORMAT GetDXTextureFormat(DXGI_FORMAT s)
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
		HookDATA* hookDATA = g_dx.GetHookDATA();
		ASSERT(hookDATA);
		if (hookDATA->Window == reinterpret_cast<HWND>(wParam))
		{
			g_dx9.Reset();
			g_dxgi.Reset();
			g_dx.m_stop.SetEvent();
			LOG(INFO) << "[GameCapture] DXCbtFilter Destory";
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
		if (!CreateEvents())
		{
			return FALSE;
		}
		if (!m_mutes[0].Create(FALSE, MUTEX_TEXTURE1, NULL))
		{
			return FALSE;
		}
		if (!m_mutes[1].Create(FALSE, MUTEX_TEXTURE2, NULL))
		{
			return FALSE;
		}
		if (!m_hookDATA.Create(SHAREDCAPTURE_MEMORY, sizeof(HookDATA)))
		{
			return FALSE;
		}
		if (!m_hookDATA.Map(0, 0))
		{
			return FALSE;
		}
		LOG(INFO) << "DX Initialize";
		return TRUE;
	}
	void DX::Uninitialize()
	{
		m_hookDATA.Close();
		m_textureDATA.Close();
		m_mutes[0].Close();
		m_mutes[1].Close();
		m_start.Close();
		m_stop.Close();
		m_targetReady.Close();
		m_sourceReady.Close();
		LOG(INFO) << "DX Uninitialize";
	}
	HookDATA* DX::GetHookDATA()
	{
		if (!m_hookDATA.Address())
		{
			if (!m_hookDATA.Open(SHAREDCAPTURE_MEMORY) && !m_hookDATA.Create(SHAREDCAPTURE_MEMORY, sizeof(HookDATA)))
			{
				return NULL;
			}
			if (!m_hookDATA.Map(0, 0))
			{
				return NULL;
			}
		}
		return reinterpret_cast<HookDATA*>(m_hookDATA.Address());
	}
	TextureDATA* DX::GetTextureDATA(DWORD dwSize)
	{
		if (m_textureDATA.GetSize() != dwSize)
		{
			m_textureDATA.Close();
		}
		if (!m_textureDATA.Address())
		{
			if (!m_textureDATA.Open(TEXTURE_MEMORY, FALSE) && !m_textureDATA.Create(TEXTURE_MEMORY, dwSize))
			{
				return NULL;
			}
			if (!m_textureDATA.Map(0, dwSize))
			{
				return NULL;
			}
		}
		return reinterpret_cast<TextureDATA*>(m_textureDATA.Address());
	}
	BOOL DX::CreateEvents()
	{
		DWORD dwProcessID = GetCurrentProcessId();
		if (!m_start.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_START, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_stop.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_STOP, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_targetReady.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_HOOK_TARGET_READY, dwProcessID).c_str()))
		{
			return FALSE;
		}
		if (!m_sourceReady.CreateEvent(FALSE, FALSE, StringPrintf("%s%d", EVENT_HOOK_SOURCE_READY, dwProcessID).c_str()))
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
	void DX::Enter()
	{
		m_lock.Lock();
	}
	void DX::Leave()
	{
		m_lock.Unlock();
	}
}