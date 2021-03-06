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
			LOG(ERROR) << "[MUTEX_TEXTURE1] Create FAIL: " << GetLastError();
			return FALSE;
		}
		if (!m_mutes[1].Create(FALSE, MUTEX_TEXTURE2, NULL))
		{
			LOG(ERROR) << "[MUTEX_TEXTURE2] Create FAIL: " << GetLastError();
			return FALSE;
		}
		if (!m_hookDATA.Create(SHAREDCAPTURE_MEMORY, sizeof(HookDATA)))
		{
			LOG(ERROR) << "[HookDATA] Create FAIL: " << GetLastError();
			return FALSE;
		}
		if (!m_hookDATA.Map(0, 0))
		{
			LOG(ERROR) << "[HookDATA] Map FAIL: " << GetLastError();
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
		return reinterpret_cast<HookDATA*>(m_hookDATA.Address());
	}
	BOOL DX::CreateTextureDATA(DWORD dwMapID, DWORD dwSize)
	{
		LOG(INFO) << "Map Name:" << dwMapID;
		m_textureDATA.Close();
		if (!m_textureDATA.Create(StringPrintf("%s%d", TEXTURE_MEMORY, dwMapID).c_str(), dwSize))
		{
			LOG(ERROR) << "TextureDATA Create FAIL:" << GetLastError();
			return FALSE;
		}
		if (!m_textureDATA.Map(0, dwSize))
		{
			LOG(ERROR) << "TextureDATA Map FAIL:" << GetLastError();
			return FALSE;
		}
		return TRUE;
	}
	TextureDATA* DX::GetTextureDATA()
	{
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
	BOOL DX::IsFrameReady(UINT64 interval)
	{
		static UINT64 lastTime = 0;
		UINT64	elapse = 0;
		UINT64	time = 0;
		if (!interval)
			return TRUE;
		time = TinyTime::Now();
		elapse = time - lastTime;
		if (elapse < interval)
		{
			return FALSE;
		}
		lastTime = (elapse > interval * 2) ? time : lastTime + interval;
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