#include "stdafx.h"
#include "DX10CaptureRunner.h"

namespace DXFramework
{
	DX10CaptureRunner::DX10CaptureRunner(DX10& dx10, DX10Image2D& image)
		:m_bCapturing(FALSE),
		m_dx10(dx10),
		m_image(image)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX10CaptureRunner::~DX10CaptureRunner()
	{

	}
	void DX10CaptureRunner::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_className = std::move(className);
		m_exeName = std::move(exeName);
		m_dllName = std::move(dllName);
	}
	BOOL DX10CaptureRunner::Submit()
	{
		return TinyThread::Submit(BindCallback(&DX10CaptureRunner::OnMessagePump, this));
	}
	BOOL DX10CaptureRunner::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyThread::Close(dwMS);
	}
	BOOL DX10CaptureRunner::OpenEvents()
	{
		string name = std::move(StringPrintf("%s%d", EVENT_CAPTURE_START, m_targetWND.dwPID));
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", EVENT_CAPTURE_STOP, m_targetWND.dwPID));
		if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", EVENT_HOOK_READY, m_targetWND.dwPID));
		if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX10CaptureRunner::CreateEvents()
	{
		string name = std::move(StringPrintf("%s%d", EVENT_CAPTURE_START, m_targetWND.dwPID));
		if (!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", EVENT_CAPTURE_STOP, m_targetWND.dwPID));
		if (!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", EVENT_HOOK_READY, m_targetWND.dwPID));
		if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	void DX10CaptureRunner::CloseEvents()
	{
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
	}
	HookDATA* DX10CaptureRunner::GetHookDATA()
	{
		if (!m_captureMemory.Address())
		{
			if (!m_captureMemory.Open(SHAREDCAPTURE_MEMORY, FALSE))
				return NULL;
			if (!m_captureMemory.Map(0, sizeof(HookDATA)))
				return NULL;
		}
		HookDATA* pDATA = reinterpret_cast<HookDATA*>(m_captureMemory.Address());
		return pDATA;
	}
	SharedTextureDATA* DX10CaptureRunner::GetSharedTextureDATA(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureMemery.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
	}
	BYTE*	DX10CaptureRunner::GetSharedTexture(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureMemery.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<LPBYTE>(m_textureMemery.Address());
	}
	BOOL CALLBACK DX10CaptureRunner::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		if (!IsWindowVisible(hwnd))
			return TRUE;
		TCHAR windowClass[MAX_PATH];
		TCHAR windowExecutable[MAX_PATH];
		if (GetClassName(hwnd, windowClass, MAX_PATH))
		{
			if (strncasecmp(windowClass, ws->className, strlen(ws->className)) == 0)
			{
				DWORD processID = 0;
				DWORD dwThreadID = GetWindowThreadProcessId(hwnd, &processID);
				if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID))
				{
					DWORD size = MAX_PATH;
					if (QueryFullProcessImageName(hProcess, 0, windowExecutable, &size))
					{
						CHAR* pzName = PathFindFileName(windowExecutable);
						if (strncasecmp(pzName, ws->exeName, strlen(pzName)) == 0)
						{
							ws->hWND = hwnd;
							ws->dwPID = processID;
							ws->dwTID = dwThreadID;
							CloseHandle(hProcess);
							hProcess = NULL;
							return FALSE;
						}
					}
					CloseHandle(hProcess);
					hProcess = NULL;
				}
			}
		}
		return TRUE;
	}
	BOOL DX10CaptureRunner::BeginCapture()
	{
		BOOL bRes = S_OK;
		if (!CreateEvents())
		{
			TRACE("BeginCapture BuildEvents-FAIL\n");
			return FALSE;
		}
		HookDATA* hookDATA = GetHookDATA();
		if (!hookDATA)
		{
			TRACE("BeginCapture GetSharedCaptureDATA-FAIL\n");
			return FALSE;
		}
		SharedTextureDATA* pTextureDATA = GetSharedTextureDATA(hookDATA->MapSize);
		if (!pTextureDATA)
		{
			TRACE("BeginCapture GetSharedTextureDATA-FAIL\n");
			return FALSE;
		}
		do
		{
			if (hookDATA->CaptureType == CAPTURETYPE_SHAREDTEXTURE)
			{
				if (!pTextureDATA->TextureHandle)
				{
					TRACE("BeginCapture GetSharedTextureDATA-TextureHandle==NULL-FAIL\n");
					return FALSE;
				}
				TinyAutoLock lock(m_lock);
				TinyRectangle vp(0, 0, static_cast<LONG>(m_image.GetSize().x), static_cast<LONG>(m_image.GetSize().y));
				m_image.Destory();
				if (!m_image.Load(m_dx10, pTextureDATA->TextureHandle))
				{
					TRACE("BeginCapture m_image.Load-FAIL\n");
					return FALSE;
				}
				m_image.SetSize(XMFLOAT2(static_cast<FLOAT>(vp.Width()), static_cast<FLOAT>(vp.Height())));
				break;
			}
			if (hookDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
			{
				TinyAutoLock lock(m_lock);
				TinyRectangle vp(0, 0, static_cast<LONG>(m_image.GetSize().x), static_cast<LONG>(m_image.GetSize().y));
				m_image.Destory();
				if (!m_image.Create(m_dx10, hookDATA->Size.cx, hookDATA->Size.cy, NULL, FALSE))
				{
					TRACE("BeginCapture m_image.Create-FAIL\n");
					return FALSE;
				}
				m_image.SetSize(XMFLOAT2(static_cast<FLOAT>(vp.Width()), static_cast<FLOAT>(vp.Height())));
				break;
			}
		} while (0);
		return TRUE;
	}
	BOOL DX10CaptureRunner::EndCapture()
	{
		TRACE("EndCapture\n");
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
		if (m_targetWND.hProcess)
		{
			CloseHandle(m_targetWND.hProcess);
			m_targetWND.hProcess = NULL;
		}
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_textureMemery.Unmap();;
		m_textureMemery.Close();
		m_captureMemory.Unmap();
		m_captureMemory.Close();
		m_bCapturing = FALSE;
		m_image.Destory();
		return TRUE;
	}
	BOOL DX10CaptureRunner::Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL bSafe)
	{
		StrCpy(m_targetWND.className, className.STR());
		StrCpy(m_targetWND.exeName, exeName.STR());
		EnumWindows(DX10CaptureRunner::EnumWindow, reinterpret_cast<LPARAM>(&m_targetWND));
		if (IsWindow(m_targetWND.hWND))
		{
			if (!m_targetWND.dwTID || !m_targetWND.dwPID)
			{
				m_bCapturing = FALSE;
				return FALSE;
			}
		}
		else
		{
			m_bCapturing = FALSE;
			return FALSE;
		}
		if (OpenEvents())
		{
			m_start.SetEvent();
			m_bCapturing = TRUE;
			return m_bCapturing;
		}
		else
		{
			if (!CreateEvents())
			{
				return FALSE;
			}
		}
		TinyProcess process;
		if (!process.Open(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwPID))
		{
			CloseEvents();
		}
		if (!InjectLibrary(process, dllName.STR()))
		{
			TRACE("InjectLibrary - FALSE\n");
			CloseEvents();
		}
		if (!process.Duplicate(m_targetWND.hProcess))
		{
			CloseEvents();
		}
		m_start.SetEvent();
		Sleep(500);
		m_bCapturing = BeginCapture();
		return m_bCapturing;
	}
	void DX10CaptureRunner::Tick()
	{
		if (m_exit && m_exit.WaitEvent(0))
		{
			TRACE("Tick - EndCapture\n");
			EndCapture();
		}
		if (m_bCapturing && !m_ready && m_targetWND.dwPID)
		{
			TRACE("Tick - CreateEvents\n");
			CreateEvents();
		}
		if (m_ready && m_ready.WaitEvent(0))
		{
			TRACE("Tick - BeginCapture\n");
			BeginCapture();
		}
		if (!m_bCapturing)
		{
			Detour(m_className, m_exeName, m_dllName);
		}
		else
		{
			if (!IsWindow(m_targetWND.hWND))
			{
				TRACE("!IsWindow(m_targetWND.hWND)\n");
				EndCapture();
			}
			if (m_targetWND.hProcess && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0)
			{
				TRACE("m_targetWND.hProcess && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0\n");
				EndCapture();
			}
		}
	}
	void DX10CaptureRunner::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.WaitEvent(5))
			{
				if (m_stop)
				{
					m_stop.SetEvent();
				}
				EndCapture();
				break;
			}
			Tick();
		}
	}
	WNDINFO	DX10CaptureRunner::GetWNDINFO()
	{
		return m_targetWND;
	}
}
