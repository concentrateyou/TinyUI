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
		return TinyTaskBase::Submit(BindCallback(&DX11CaptureTask::MessagePump, this));
	}
	BOOL DX11CaptureTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	DWORD DX11CaptureTask::BuildEvents()
	{
		if (!m_captureStart)
		{
			string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
			if (!m_captureStart.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureStop)
		{
			string name = StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID);
			if (!m_captureStop.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureReady)
		{
			string name = StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID);
			if (!m_captureReady.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureExit)
		{
			string name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID);
			if (!m_captureExit.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	SharedCaptureDATA* DX11CaptureTask::GetSharedCaptureDATA()
	{
		if (!m_memory.Address())
		{
			if (!m_memory.Open(SHAREDCAPTURE_MEMORY, FALSE))
				return FALSE;
			if (!m_memory.Map(0, sizeof(SharedCaptureDATA)))
				return FALSE;
		}
		SharedCaptureDATA* pDATA = reinterpret_cast<SharedCaptureDATA*>(m_memory.Address());
		return pDATA;
	}
	SharedTextureDATA* DX11CaptureTask::GetSharedTextureDATA()
	{
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return FALSE;
			if (!m_textureMemery.Map(0, sizeof(SharedTextureDATA)))
				return FALSE;
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
				DWORD processID;
				GetWindowThreadProcessId(hwnd, &processID);
				if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID))
				{
					DWORD size = MAX_PATH;
					if (QueryFullProcessImageName(hProcess, 0, windowExecutable, &size))
					{
						CHAR* pzName = PathFindFileName(windowExecutable);
						if (strncasecmp(pzName, ws->exeName, strlen(pzName)) == 0)
						{
							ws->hWND = hwnd;
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
		if (!BuildEvents())
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
		if (!pTextureDATA)
		{
			TRACE("BeginCapture GetSharedTextureDATA-FAIL\n");
			return FALSE;
		}
		if (!pTextureDATA->TextureHandle ||
			!m_image.Load(*m_pDX11, pTextureDATA->TextureHandle))
		{
			TRACE("BeginCapture m_image.Load-FAIL\n");
			return FALSE;
		}

		return TRUE;
	}
	BOOL DX11CaptureTask::EndCapture()
	{
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
		m_bCapturing = FALSE;
		return TRUE;
	}
	BOOL DX11CaptureTask::AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		HANDLE hProcess = NULL;
		HWND hWND = ::FindWindow(className.STR(), NULL);
		if (!hWND)
		{
			WNDINFO ws = { 0 };
			StrCpy(ws.className, className.STR());
			StrCpy(ws.exeName, exeName.STR());
			EnumWindows(DX11CaptureTask::EnumWindow, reinterpret_cast<LPARAM>(&ws));
		}
		if (hWND)
		{
			TRACE("AttemptCapture hWND != NULL\n");
			m_targetWND.hWND = hWND;
			m_targetWND.dwThreadID = GetWindowThreadProcessId(hWND, &m_targetWND.dwProcessID);
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
		if (!BuildEvents())
		{
			TRACE("BuildEvents == FALSE\n");
			goto _ERROR;
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
			TRACE("Tick - BuildEvents\n");
			BuildEvents();
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
	void DX11CaptureTask::MessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(15))
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID);
				if (hProcess)
				{
					UninjectLibrary(hProcess, m_dllName.STR());
					CloseHandle(hProcess);
				}
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
