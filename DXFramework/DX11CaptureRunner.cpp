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
		m_mutes[0].Create(FALSE, MUTEX_TEXTURE1, NULL);
		m_mutes[1].Create(FALSE, MUTEX_TEXTURE2, NULL);
		m_textures[0] = m_textures[1] = NULL;
		ZeroMemory(&m_target, sizeof(m_target));
	}
	DX11CaptureRunner::~DX11CaptureRunner()
	{

	}
	void DX11CaptureRunner::SetConfig(const TinyString& className, const TinyString& exeName)
	{
		m_szClass = std::move(className);
		m_szEXE = std::move(exeName);
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
	}
	HookDATA* DX11CaptureRunner::GetHookDATA()
	{
		return reinterpret_cast<HookDATA*>(m_hookDATA.Address());
	}
	TextureDATA* DX11CaptureRunner::GetTextureDATA()
	{
		return reinterpret_cast<TextureDATA*>(m_textureDATA.Address());
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
		if (!InitializeDATA())
		{
			TRACE("[InitializeDATA] FAIL\n");
			StopCapture();
			return FALSE;
		}
		HookDATA* hookDATA = GetHookDATA();
		if (!hookDATA)
		{
			TRACE("[StartCapture] GetHookDATA = NULL\n");
			return FALSE;
		}
		TextureDATA* textureDATA = GetTextureDATA();
		if (!textureDATA)
		{
			TRACE("[StartCapture] GetTextureDATA = NULL\n");
			return FALSE;
		}
		do
		{
			if (hookDATA->CaptureType == CAPTURETYPE_SHAREDTEXTURE)
			{
				if (!textureDATA->TextureHandle)
				{
					return FALSE;
				}
				m_image2D.Destory();
				if (!m_image2D.Load(*m_pDX11, textureDATA->TextureHandle))
				{
					return FALSE;
				}
				break;
			}
			if (hookDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
			{
				m_image2D.Destory();
				TRACE("Image2D: %d,%d\n", hookDATA->Size.cx, hookDATA->Size.cy);
				if (!m_image2D.Create(*m_pDX11, hookDATA->Size.cx, hookDATA->Size.cy, NULL, FALSE))
				{
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
	BOOL DX11CaptureRunner::CopyTextureDATA()
	{
		HookDATA* hookDATA = GetHookDATA();
		if (!hookDATA)
			return FALSE;
		TextureDATA* textureDATA = GetTextureDATA();
		if (!textureDATA)
			return FALSE;
		switch (hookDATA->CaptureType)
		{
		case CAPTURETYPE_SHAREDTEXTURE:
			return TRUE;
		case CAPTURETYPE_MEMORYTEXTURE:
		{
			if (textureDATA != NULL)
			{
				DWORD dwCurrentID = textureDATA->CurrentID;
				BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
				m_textures[0] = address + textureDATA->Texture1Offset;
				m_textures[1] = address + textureDATA->Texture2Offset;
				do
				{
					XMFLOAT2 size = m_image2D.GetSize();
					if (static_cast<INT32>(size.x) != hookDATA->Size.cx || static_cast<INT32>(size.y) != hookDATA->Size.cy)
					{
						TRACE("ImageSize: %f,%f, NewSize:%d,%d\n", size.x, size.y, hookDATA->Size.cx, hookDATA->Size.cy);
						return FALSE;
					}
					else
					{
						DWORD dwNextID = (dwCurrentID == 1) ? 0 : 1;
						if (m_mutes[dwCurrentID].Lock(0))
						{
							D3D11_MAPPED_SUBRESOURCE ms = { 0 };
							if (m_image2D.Map(*m_pDX11, ms, FALSE))
							{
								if (hookDATA->Pitch == ms.RowPitch)
								{
									memcpy(ms.pData, m_textures[dwCurrentID], hookDATA->Pitch * static_cast<INT32>(size.y));
								}
								else
								{
									UINT32 bestPitch = std::min<UINT32>(hookDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwCurrentID];
									for (INT32 y = 0; y < static_cast<INT32>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (hookDATA->Pitch*y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								m_image2D.Unmap(*m_pDX11);
							}
							m_mutes[dwCurrentID].Unlock();
							break;
						}
						if (m_mutes[dwNextID].Lock(0))
						{
							D3D11_MAPPED_SUBRESOURCE ms = { 0 };
							if (m_image2D.Map(*m_pDX11, ms, FALSE))
							{
								XMFLOAT2 size = m_image2D.GetSize();
								if (hookDATA->Pitch == ms.RowPitch)
								{
									memcpy(ms.pData, m_textures[dwNextID], hookDATA->Pitch * static_cast<INT32>(size.y));
								}
								else
								{
									UINT32 bestPitch = std::min<UINT32>(hookDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwNextID];
									for (INT32 y = 0; y < static_cast<INT32>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (hookDATA->Pitch * y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								m_image2D.Unmap(*m_pDX11);
							}
							m_mutes[dwNextID].Unlock();
							break;
						}
					}
				} while (0);
				return TRUE;
			}
		}
		break;

		}
		return FALSE;
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
		TRACE("Detour\n");
		TinyProcess process;
		if (!process.Open(PROCESS_ALL_ACCESS, FALSE, m_target.dwPID))
		{
			m_bActive = FALSE;
			return FALSE;
		}
		if (!AttemptExisting())
		{
			TRACE("AttemptExisting FALSE\n");
			if (!AttemptDetour(anticheat))
			{
				TRACE("AttemptDetour FALSE\n");
				m_bActive = FALSE;
				return FALSE;
			}
			else
			{
				TRACE("AttemptExisting TRUE\n");
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
	BOOL DX11CaptureRunner::Tick(INT64& timestamp)
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
		if (m_targetReady.WaitEvent(0))//目标进程准备就绪
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
			return CopyTextureDATA();
		}
		return FALSE;
	}
	void DX11CaptureRunner::Close()
	{
		StopCapture();
	}
	WNDINFO	DX11CaptureRunner::GetWNDINFO()
	{
		return m_target;
	}
	BOOL DX11CaptureRunner::InitializeDATA()
	{
		m_hookDATA.Close();
		if (!m_hookDATA.Open(SHAREDCAPTURE_MEMORY, FALSE))
		{
			TRACE("[HookDATA] Open FAIL: %d\n", GetLastError());
			return FALSE;
		}
		if (!m_hookDATA.Map(0, sizeof(HookDATA)))
		{
			TRACE("[HookDATA] Map FAIL: %d\n", GetLastError());
			return FALSE;
		}
		HookDATA* pDATA = reinterpret_cast<HookDATA*>(m_hookDATA.Address());
		m_textureDATA.Close();
		TRACE("[InitializeDATA] New MapID:%d\n", pDATA->MapID);
		if (!m_textureDATA.Open(StringPrintf("%s%d", TEXTURE_MEMORY, pDATA->MapID), FALSE))
		{
			TRACE("[TextureDATA] Open %d FAIL: %d\n", pDATA->MapID, GetLastError());
			return FALSE;
		}
		if (!m_textureDATA.Map(0, pDATA->MapSize))
		{
			TRACE("[TextureDATA] Map %d FAIL: %d\n", pDATA->MapSize, GetLastError());
			return FALSE;
		}
		return TRUE;
	}
}
