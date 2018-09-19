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
		inject += ws.bX86 ? "Inject32.exe" : "Inject64.exe";
		if (PathFileExists(inject.c_str()))
			return  FALSE;
		string dll;
		dll.resize(MAX_PATH);
		GetModuleFileName(NULL, &dll[0], MAX_PATH);
		dll = dll.substr(0, dll.find_last_of("\\", string::npos, 1));
		dll += ws.bX86 ? "GameDetour32.dll" : "GameDetour64.dll";
		if (PathFileExists(dll.c_str()))
			return  FALSE;
		vector<string> args;
		args.push_back(inject);
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
	BOOL GameInject::GetExitCode(DWORD& exit)
	{
		return ::GetExitCodeProcess(m_process, &exit);
	}
	void GameInject::Close()
	{
		m_process.Close();
	}
	//////////////////////////////////////////////////////////////////////////
	DX11CaptureRunner::DX11CaptureRunner(DX11* pDX11, DX11Image2D& image)
		: m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_pDX11(pDX11),
		m_image2D(image)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11CaptureRunner::~DX11CaptureRunner()
	{

	}
	void DX11CaptureRunner::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_szClass = std::move(className);
		m_szEXE = std::move(exeName);
		m_szDLL = std::move(dllName);
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
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_START, m_targetWND.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_CAPTURE_STOP, m_targetWND.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_HOOK_READY, m_targetWND.dwPID).c_str()))
		{
			return FALSE;
		}
		if (!m_init.OpenEvent(EVENT_ALL_ACCESS, FALSE, StringPrintf("%s%d", EVENT_HOOK_INIT, m_targetWND.dwPID).c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}

	void DX11CaptureRunner::CloseEvents()
	{
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_init.Close();
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
	BOOL DX11CaptureRunner::Start()
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
	void DX11CaptureRunner::Stop()
	{
		m_stop.SetEvent();
		m_textureDATA.Close();
		m_hookDATA.Close();
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_init.Close();
		m_inject.Close();
		SAFE_CLOSE_HANDLE(m_targetWND.hProcess);
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_bCapturing = FALSE;
		m_image2D.Destory();
	}
	BOOL DX11CaptureRunner::Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL anticheat)
	{
		StrCpy(m_targetWND.className, className.STR());
		StrCpy(m_targetWND.exeName, exeName.STR());
		EnumWindows(DX11CaptureRunner::EnumWindow, reinterpret_cast<LPARAM>(&m_targetWND));
		if (!IsWindow(m_targetWND.hWND))
		{
			m_bCapturing = FALSE;
			return FALSE;
		}
		TinyProcess process;
		if (!process.Open(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwPID))
		{
			CloseEvents();
			m_bCapturing = FALSE;
			return FALSE;
		}
		if (OpenEvents())
		{
			m_start.SetEvent();
			m_bCapturing = TRUE;
			return m_bCapturing;
		}
		if (!m_inject.Open(m_targetWND, anticheat))
		{
			CloseEvents();
			m_bCapturing = FALSE;
			return FALSE;
		}
		m_init.SetEvent();
		m_bActive = TRUE;
		m_bCapturing = Start();
		return m_bCapturing;
	}
	void DX11CaptureRunner::Tick()
	{
		if (m_stop.WaitEvent(0))
		{
			Stop();
		}
		if (m_bActive && !m_ready && m_targetWND.dwPID > 0)
		{
			OpenEvents();
		}
		if (m_inject.Wait(0))
		{
			DWORD code = 0;
			m_inject.GetExitCode(code);
			m_inject.Close();
			if (code == 0)
			{
				if (!m_bCapturing)
				{
					Stop();
				}
			}
		}
		if (m_ready.WaitEvent(0))
		{
			m_bCapturing = Start();
		}
		if (!m_bActive)
		{
			Detour(m_szClass, m_szEXE, m_szDLL);
		}
		else
		{
			if (!IsWindow(m_targetWND.hWND))
			{
				Stop();
			}
			if (m_targetWND.hProcess != NULL && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0)
			{
				Stop();
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
				Stop();
				break;
			}
			Tick();
		}
	}
	WNDINFO	DX11CaptureRunner::GetWNDINFO()
	{
		return m_targetWND;
	}
}
