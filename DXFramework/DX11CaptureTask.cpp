#include "stdafx.h"
#include "DX11CaptureTask.h"

namespace DXFramework
{
	DX11CaptureTask::DX11CaptureTask(DX11* pDX11, DX11Image& image)
		:m_bCapturing(FALSE),
		m_pDX11(pDX11),
		m_image(image)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11CaptureTask::~DX11CaptureTask()
	{

	}
	void DX11CaptureTask::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_className = std::move(className);
		m_exeName = std::move(exeName);
		m_dllName = std::move(dllName);
	}
	BOOL DX11CaptureTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&DX11CaptureTask::OnMessagePump, this));
	}
	BOOL DX11CaptureTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	BOOL DX11CaptureTask::OpenEvents()
	{
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_captureStart.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_captureStop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID));
		if (!m_captureReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID));
		if (!m_captureExit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX11CaptureTask::CreateEvents()
	{
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_captureStart.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID));
		if (!m_captureStop.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID));
		if (!m_captureReady.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID));
		if (!m_captureExit.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	SharedCaptureDATA* DX11CaptureTask::GetSharedCaptureDATA()
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
	SharedTextureDATA* DX11CaptureTask::GetSharedTextureDATA()
	{
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureMemery.Map(0, sizeof(SharedTextureDATA)))
				return NULL;
		}
		SharedTextureDATA* pDATA = reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
		return pDATA;
	}

	BOOL CALLBACK DX11CaptureTask::EnumWindow(HWND hwnd, LPARAM lParam)
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
	BOOL DX11CaptureTask::BeginCapture()
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
		SharedTextureDATA* pTextureDATA = GetSharedTextureDATA();
		if (!pTextureDATA || !pTextureDATA->TextureHandle)
		{
			TRACE("BeginCapture GetSharedTextureDATA-FAIL\n");
			return FALSE;
		}
		m_pDX11->Lock();
		m_image.Destory();
		if (!m_image.Load(*m_pDX11, pTextureDATA->TextureHandle))
		{
			m_pDX11->Unlock();
			TRACE("BeginCapture m_image.Load-FAIL\n");
			return FALSE;
		}
		m_pDX11->Unlock();
		return TRUE;
	}
	BOOL DX11CaptureTask::EndCapture()
	{
		TRACE("EndCapture\n");
		m_captureStart.Close();
		m_captureStop.Close();
		m_captureReady.Close();
		m_captureExit.Close();
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
		return TRUE;
	}
	BOOL DX11CaptureTask::AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		HANDLE hProcess = NULL;
		StrCpy(m_targetWND.className, className.STR());
		StrCpy(m_targetWND.exeName, exeName.STR());
		EnumWindows(DX11CaptureTask::EnumWindow, reinterpret_cast<LPARAM>(&m_targetWND));
		if (m_targetWND.hWND)
		{
			TRACE("AttemptCapture hWND != NULL\n");
			if (!m_targetWND.dwThreadID || !m_targetWND.dwProcessID)
			{
				TRACE("!m_targetWND.dwThreadID || !m_targetWND.dwProcessID\n");
				m_bCapturing = FALSE;
				goto _ERROR;
			}
		}
		else
		{
			m_bCapturing = FALSE;
			goto _ERROR;
		}

		if (OpenEvents())
		{
			m_captureStart.SetEvent();
			m_bCapturing = TRUE;
			return m_bCapturing;
		}
		else
		{
			if (!CreateEvents())
			{
				TRACE("BuildEvents == FALSE\n");
				goto _ERROR;
			}
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID);
		if (!hProcess)
		{
			TRACE("hProcess == NULL\n");
			goto _ERROR;
		}
		TRACE("Begin InjectLibrary\n");
		if (!InjectLibrary(hProcess, dllName.STR()))
		{
			TRACE("InjectLibrary - FALSE\n");
			goto _ERROR;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_targetWND.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			TRACE("DuplicateHandle - FALSE\n");
			goto _ERROR;
		}
		m_captureStart.SetEvent();
		Sleep(500);
		m_bCapturing = BeginCapture();
		if (m_bCapturing)
		{
			TRACE("AttemptCapture - m_bCapturing = TRUE\n");
		}
	_ERROR:
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return m_bCapturing;
	}
	void DX11CaptureTask::Tick()
	{
		if (m_captureExit && m_captureExit.Lock(0))
		{
			TRACE("Tick - EndCapture\n");
			EndCapture();
		}
		if (m_bCapturing && !m_captureReady && m_targetWND.dwProcessID)
		{
			TRACE("Tick - CreateEvents\n");
			CreateEvents();
		}
		if (m_captureReady && m_captureReady.Lock(0))
		{
			TRACE("Tick - BeginCapture\n");
			BeginCapture();
		}
		if (!m_bCapturing)
		{
			AttemptCapture(m_className, m_exeName, m_dllName);
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
	void DX11CaptureTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(15))
			{
				if (m_captureStop)
					m_captureStop.SetEvent();
				EndCapture();
				break;
			}
			Tick();
		}
	}
	WNDINFO	DX11CaptureTask::GetWNDINFO()
	{
		return m_targetWND;
	}
}
