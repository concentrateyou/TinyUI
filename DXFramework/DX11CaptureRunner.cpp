#include "stdafx.h"
#include "DX11CaptureRunner.h"

namespace DXFramework
{
	DX11CaptureRunner::DX11CaptureRunner(DX11* pDX11, DX11Image2D& image)
		:m_bCapturing(FALSE),
		m_pDX11(pDX11),
		m_image(image)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11CaptureRunner::~DX11CaptureRunner()
	{

	}
	void DX11CaptureRunner::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_className = std::move(className);
		m_exeName = std::move(exeName);
		m_dllName = std::move(dllName);
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
		string name = std::move(StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID));
		if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID));
		if (!m_exit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX11CaptureRunner::CreateEvents()
	{
		string name = std::move(StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID));
		if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID));
		if (!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
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
		m_exit.Close();
	}
	SharedCaptureDATA* DX11CaptureRunner::GetSharedCaptureDATA()
	{
		if (!m_captureMemory.Address())
		{
			if (!m_captureMemory.Open(SHAREDCAPTURE_MEMORY, FALSE))
				return NULL;
			if (!m_captureMemory.Map(0, sizeof(SharedCaptureDATA)))
				return NULL;
		}
		SharedCaptureDATA* pDATA = reinterpret_cast<SharedCaptureDATA*>(m_captureMemory.Address());
		return pDATA;
	}
	SharedTextureDATA* DX11CaptureRunner::GetSharedTextureDATA(DWORD dwSize)
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
	BYTE*	DX11CaptureRunner::GetSharedTexture(DWORD dwSize)
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
	BOOL CALLBACK DX11CaptureRunner::EnumWindow(HWND hwnd, LPARAM lParam)
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
							ws->dwProcessID = processID;
							ws->dwThreadID = dwThreadID;
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
	BOOL DX11CaptureRunner::IsCapturing() const
	{
		return m_bCapturing;
	}
	BOOL DX11CaptureRunner::BeginCapture()
	{
		ASSERT(m_pDX11);
		BOOL bRes = S_OK;
		if (!CreateEvents())
		{
			TRACE("BeginCapture BuildEvents-FAIL\n");
			return FALSE;
		}
		SharedCaptureDATA* pCaptureDATA = GetSharedCaptureDATA();
		if (!pCaptureDATA)
		{
			TRACE("BeginCapture GetSharedCaptureDATA-FAIL\n");
			return FALSE;
		}
		SharedTextureDATA* pTextureDATA = GetSharedTextureDATA(pCaptureDATA->MapSize);
		if (!pTextureDATA)
		{
			TRACE("BeginCapture GetSharedTextureDATA-FAIL\n");
			return FALSE;
		}
		do
		{
			if (pCaptureDATA->CaptureType == CAPTURETYPE_SHAREDTEXTURE)
			{
				if (!pTextureDATA->TextureHandle)
				{
					TRACE("BeginCapture GetSharedTextureDATA-TextureHandle==NULL-FAIL\n");
					return FALSE;
				}
				TinyAutoLock autolock(m_lock);
				m_image.Destory();
				if (!m_image.Load(*m_pDX11, pTextureDATA->TextureHandle))
				{
					TRACE("BeginCapture m_image.Load-FAIL\n");
					return FALSE;
				}
				break;
			}
			if (pCaptureDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
			{
				TinyAutoLock autolock(m_lock);
				m_image.Destory();
				if (!m_image.Create(*m_pDX11, pCaptureDATA->Size.cx, pCaptureDATA->Size.cy, NULL, FALSE))
				{
					TRACE("BeginCapture m_image.Create-FAIL\n");
					return FALSE;
				}
				m_image.SetTranslate(XMFLOAT2(0.0F, 0.0F));
				m_image.SetScale(XMFLOAT2(1.0F, 1.0F));
				break;
			}
		} while (0);
		return TRUE;
	}
	BOOL DX11CaptureRunner::EndCapture()
	{
		TRACE("EndCapture\n");
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
		if (m_targetWND.hProcess != NULL)
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
	BOOL DX11CaptureRunner::Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL bSafe)
	{
		StrCpy(m_targetWND.className, className.STR());
		StrCpy(m_targetWND.exeName, exeName.STR());
		EnumWindows(DX11CaptureRunner::EnumWindow, reinterpret_cast<LPARAM>(&m_targetWND));
		if (IsWindow(m_targetWND.hWND))
		{
			if (!m_targetWND.dwThreadID || !m_targetWND.dwProcessID)
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
		if (!process.Open(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID))
		{
			CloseEvents();
		}
		if (bSafe)
		{
			if (!InjectLibrarySafe(m_targetWND.dwThreadID, dllName.STR()))
			{
				TRACE("InjectLibrarySafe - FALSE\n");
				CloseEvents();
			}
		}
		else
		{
			if (!InjectLibrary(process, dllName.STR()))
			{
				TRACE("InjectLibrary - FALSE\n");
				CloseEvents();
			}
		}
		if (!DuplicateHandle(GetCurrentProcess(), process, GetCurrentProcess(), &m_targetWND.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			CloseEvents();
		}
		m_start.SetEvent();
		m_bCapturing = BeginCapture();
		return m_bCapturing;
	}
	void DX11CaptureRunner::Tick()
	{
		if (m_exit.WaitEvent(0))
		{
			TRACE("Tick - EndCapture\n");
			EndCapture();
		}
		if (m_bCapturing && !m_ready && m_targetWND.dwProcessID)
		{
			TRACE("Tick - CreateEvents\n");
			CreateEvents();
		}
		if (m_ready.WaitEvent(0))
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
				EndCapture();
			}
			if (m_targetWND.hProcess != NULL && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0)
			{
				TRACE("m_targetWND.hProcess && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0\n");
				EndCapture();
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
				EndCapture();
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
