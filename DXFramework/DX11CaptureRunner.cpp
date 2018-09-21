#include "stdafx.h"
#include "DX11CaptureRunner.h"

namespace DXFramework
{
	GameInject::GameInject()
	{

	}
	GameInject::~GameInject()
	{

	}
	BOOL GameInject::Open(const WNDINFO& ws, BOOL anticheat)
	{
		string inject;
		inject.resize(MAX_PATH);
		GetModuleFileName(NULL, &inject[0], MAX_PATH);
		inject = inject.substr(0, inject.find_last_of("\\", string::npos, 1));
		inject += ws.bX86 ? "\\Inject32.exe" : "\\Inject64.exe";
		if (!PathFileExists(inject.c_str()))
			return  FALSE;
		string dll;
		dll = ws.bX86 ? "GameDetour32.dll" : "GameDetour64.dll";
		vector<string> args;
		args.push_back(dll);
		args.push_back(std::to_string(anticheat));
		args.push_back(std::to_string(ws.dwTID));
		args.push_back(std::to_string(ws.dwPID));
		if (!m_process.Create(inject, args))
			return FALSE;
		return TRUE;
	}
	BOOL GameInject::Wait(DWORD dwMS)
	{
		return m_process.Wait(dwMS);
	}
	BOOL GameInject::Close()
	{
		DWORD exit = 0;
		::GetExitCodeProcess(m_process, &exit);
		m_process.Close();
		return exit == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	DX11CaptureRunner::DX11CaptureRunner(DX11* pDX11, DX11Image2D& image)
		: m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_pDX11(pDX11),
		m_image2D(image)
	{
		ZeroMemory(&m_target, sizeof(m_target));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11CaptureRunner::~DX11CaptureRunner()
	{

	}
	void DX11CaptureRunner::SetConfig(const TinyString& className, const TinyString& exeName)
	{
		m_szClass = std::move(className);
		m_szEXE = std::move(exeName);
	}
	BOOL DX11CaptureRunner::Submit()
	{
		return TinyThread::Submit(BindCallback(&DX11CaptureRunner::OnMessagePump, this));
	}
	BOOL DX11CaptureRunner::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyThread::Close(dwMS);
	}
	BOOL DX11CaptureRunner::OpenEvents()
	{
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_START, m_target.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_STOP, m_target.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_targetReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_HOOK_TARGET_READY, m_target.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_sourceReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_HOOK_SOURCE_READY, m_target.dwPID).c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}

	void DX11CaptureRunner::CloseEvents()
	{
		m_start.Close();
		m_stop.Close();
		m_targetReady.Close();
		m_sourceReady.Close();
		m_close.Close();
	}
	HookDATA* DX11CaptureRunner::GetHookDATA()
	{
		if (!m_hookDATA.Address())
		{
			if (!m_hookDATA.Open(SHAREDCAPTURE_MEMORY, FALSE))
				return NULL;
			if (!m_hookDATA.Map(0, sizeof(HookDATA)))
				return NULL;
		}
		HookDATA* pDATA = reinterpret_cast<HookDATA*>(m_hookDATA.Address());
		return pDATA;
	}
	SharedTextureDATA* DX11CaptureRunner::GetSharedTextureDATA(DWORD dwSize)
	{
		if (m_textureDATA.GetSize() != dwSize)
		{
			m_textureDATA.Close();
		}
		if (!m_textureDATA.Address())
		{
			if (!m_textureDATA.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureDATA.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<SharedTextureDATA*>(m_textureDATA.Address());
	}
	BYTE*	DX11CaptureRunner::GetSharedTexture(DWORD dwSize)
	{
		if (m_textureDATA.GetSize() != dwSize)
		{
			m_textureDATA.Close();
		}
		if (!m_textureDATA.Address())
		{
			if (!m_textureDATA.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureDATA.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<LPBYTE>(m_textureDATA.Address());
	}
	BOOL CALLBACK DX11CaptureRunner::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		if (!IsWindowVisible(hwnd))
			return TRUE;
		TCHAR szClass[MAX_PATH];
		TCHAR szExecutable[MAX_PATH];
		if (GetClassName(hwnd, szClass, MAX_PATH))
		{
			if (strncasecmp(szClass, ws->className, strlen(ws->className)) == 0)
			{
				DWORD dwPID = 0;
				DWORD dwTID = GetWindowThreadProcessId(hwnd, &dwPID);
				TinyProcess process;
				if (process.Open(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID))
				{
					DWORD size = MAX_PATH;
					if (QueryFullProcessImageName(process, 0, szExecutable, &size))
					{
						CHAR* pzName = PathFindFileName(szExecutable);
						if (strncasecmp(pzName, ws->exeName, strlen(pzName)) == 0)
						{
							ws->hWND = hwnd;
							ws->dwPID = dwPID;
							ws->dwTID = dwTID;
							IsWow64Process(process, &ws->bX86);
							ws->hProcess = process.Duplicate();
							return FALSE;
						}
					}
				}
			}
		}
		return TRUE;
	}
	BOOL DX11CaptureRunner::IsCapturing() const
	{
		return m_bCapturing;
	}
	BOOL DX11CaptureRunner::StartCapture()
	{
		ASSERT(m_pDX11);
		BOOL bRes = S_OK;
		if (!OpenEvents())
		{
			LOG(ERROR) << "[Start] OpenEvents FAIL";
			return FALSE;
		}
		HookDATA* hookDATA = GetHookDATA();
		if (!hookDATA)
		{
			LOG(ERROR) << "[Start] GetHookDATA FAIL";
			return FALSE;
		}
		SharedTextureDATA* pTextureDATA = GetSharedTextureDATA(hookDATA->MapSize);
		if (!pTextureDATA)
		{
			LOG(ERROR) << "[Start] GetSharedTextureDATA FAIL";
			return FALSE;
		}
		do
		{
			if (hookDATA->CaptureType == CAPTURETYPE_SHAREDTEXTURE)
			{
				if (!pTextureDATA->TextureHandle)
				{
					LOG(ERROR) << "[Start] TextureHandle == NULL";
					return FALSE;
				}
				TinyAutoLock autolock(m_lock);
				m_image2D.Destory();
				if (!m_image2D.Load(*m_pDX11, pTextureDATA->TextureHandle))
				{
					LOG(ERROR) << "[Start] load image FAIL";
					return FALSE;
				}
				break;
			}
			if (hookDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
			{
				TinyAutoLock autolock(m_lock);
				m_image2D.Destory();
				if (!m_image2D.Create(*m_pDX11, hookDATA->Size.cx, hookDATA->Size.cy, NULL, FALSE))
				{
					LOG(ERROR) << "[Start] create image FAIL";
					return FALSE;
				}
				m_image2D.SetTranslate(XMFLOAT2(0.0F, 0.0F));
				m_image2D.SetScale(XMFLOAT2(1.0F, 1.0F));
				break;
			}
		} while (0);
		return TRUE;
	}
	void DX11CaptureRunner::StopCapture()
	{
		m_stop.SetEvent();
		m_textureDATA.Close();
		m_hookDATA.Close();
		m_start.Close();
		m_stop.Close();
		m_targetReady.Close();
		m_sourceReady.Close();
		m_injector.Close();
		SAFE_CLOSE_HANDLE(m_target.hProcess);
		ZeroMemory(&m_target, sizeof(m_target));
		m_bCapturing = FALSE;
		m_bActive = FALSE;
		m_image2D.Destory();
	}
	BOOL DX11CaptureRunner::AttemptExisting()
	{
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_START, m_target.dwPID).c_str()))
		{
			return FALSE;
		}
		m_start.SetEvent();
		return TRUE;
	}
	BOOL DX11CaptureRunner::AttemptDetour(BOOL anticheat)
	{
		return m_injector.Open(m_target, anticheat);
	}
	BOOL DX11CaptureRunner::Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL anticheat)
	{
		StrCpy(m_target.className, className.STR());
		StrCpy(m_target.exeName, exeName.STR());
		EnumWindows(DX11CaptureRunner::EnumWindow, reinterpret_cast<LPARAM>(&m_target));
		if (!IsWindow(m_target.hWND))
		{
			m_bActive = FALSE;
			return FALSE;
		}
		TinyProcess process;
		if (!process.Open(PROCESS_ALL_ACCESS, FALSE, m_target.dwPID))
		{
			m_bActive = FALSE;
			return FALSE;
		}
		if (!AttemptExisting())
		{
			if (!AttemptDetour(anticheat))
			{
				m_bActive = FALSE;
				return FALSE;
			}
		}
		if (!OpenEvents())
		{
			m_bActive = FALSE;
			return FALSE;
		}
		m_sourceReady.SetEvent();
		m_bActive = TRUE;
		return TRUE;
	}
	void DX11CaptureRunner::Tick()
	{
		if (m_stop.WaitEvent(0))//停止捕获
		{
			StopCapture();
		}
		if (m_bActive && !m_targetReady && m_target.dwPID > 0)
		{
			m_targetReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_HOOK_TARGET_READY, m_target.dwPID).c_str());
		}
		if (m_injector.Wait(0))
		{
			if (m_injector.Close())
			{
				if (!m_bCapturing)
				{
					StopCapture();
				}
			}
		}
		if (m_targetReady.WaitEvent(0))//服务端准备就系
		{
			m_bCapturing = StartCapture();
		}
		if (!m_bActive)
		{
			Detour(m_szClass, m_szEXE, m_szDLL);
		}
		else
		{
			if (!IsWindow(m_target.hWND))
			{
				StopCapture();
			}
			if (m_target.hProcess != NULL && WaitForSingleObject(m_target.hProcess, 0) == WAIT_OBJECT_0)
			{
				StopCapture();
			}
		}
	}
	void DX11CaptureRunner::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.WaitEvent(5))
			{
				if (m_stop)
				{
					m_stop.SetEvent();
				}
				StopCapture();
				break;
			}
			Tick();
		}
	}
	WNDINFO	DX11CaptureRunner::GetWNDINFO()
	{
		return m_target;
	}
}
