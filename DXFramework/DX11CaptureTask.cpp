#include "stdafx.h"
#include "DX11CaptureTask.h"

namespace DXFramework
{
	DX11CaptureTask::DX11CaptureTask(DX11* pDX11, INT cx, INT cy)
		:m_bCapturing(FALSE),
		m_pDX11(pDX11),
		m_cx(cx),
		m_cy(cy)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11CaptureTask::~DX11CaptureTask()
	{

	}
	BOOL DX11CaptureTask::Submit()
	{
		Closure s = BindCallback(&DX11CaptureTask::MessagePump, this);
		return TinyTaskBase::Submit(s);
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
			if (!m_captureStart.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_captureStart.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureStop)
		{
			string name = StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID);
			if (!m_captureStop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_captureStop.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureReady)
		{
			string name = StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID);
			if (!m_captureReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_captureReady.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_captureExit)
		{
			string name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID);
			if (!m_captureExit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_captureExit.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	SharedCaptureDATA* DX11CaptureTask::GetSharedCapture()
	{
		m_memory.Unmap();
		if (m_memory.Open(SHAREDCAPTURE_MEMORY) &&
			m_memory.Map())
		{
			return reinterpret_cast<SharedCaptureDATA*>(m_memory.Address());
		}
		return NULL;
	}
	BOOL CALLBACK DX11CaptureTask::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		if (!IsWindowVisible(hwnd))
			return TRUE;
		TCHAR windowClass[MAX_PATH];
		TCHAR windowExecutable[MAX_PATH];
		if (GetClassName(hwnd, windowClass, MAX_PATH) &&
			strncasecmp(windowClass, ws->className, strlen(ws->className)) == 0)
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
		return TRUE;
	}
	BOOL DX11CaptureTask::BeginCapture()
	{
		ASSERT(m_pDX11);
		BOOL bRes = S_OK;
		if (!BuildEvents())
			return FALSE;
		SharedCaptureDATA* pDATA = GetSharedCapture();
		if (!pDATA)
			return FALSE;
		if (!m_texture.Initialize(*m_pDX11, m_cx, m_cy))
			return FALSE;
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
			m_targetWND.hWND = hWND;
			m_targetWND.dwThreadID = GetWindowThreadProcessId(hWND, &m_targetWND.dwProcessID);
			if (!m_targetWND.dwThreadID || !m_targetWND.dwProcessID)
			{
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
			goto _ERROR;
		}
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID);
		if (!hProcess)
		{
			goto _ERROR;
		}
		if (!InjectLibrary(hProcess, dllName.STR()))
		{
			goto _ERROR;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_targetWND.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			goto _ERROR;
		}
		m_captureStart.SetEvent();
		Sleep(300);
		m_bCapturing = BeginCapture();
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
			EndCapture();
		}
		if (m_bCapturing && !m_captureReady && m_targetWND.dwProcessID)
		{
			BuildEvents();
		}
		if (m_captureReady && m_captureReady.Lock(0))
		{
			BeginCapture();
		}
		if (!m_bCapturing)
		{
			AttemptCapture(TEXT("Warcraft III"), TEXT("war3.exe"), TEXT("D:\\Develop\\TinyUI\\Debug\\GameDetour.dll"));
		}
		else
		{
			if (!IsWindow(m_targetWND.hWND))
			{
				EndCapture();
			}
			if (m_targetWND.hProcess && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0)
			{
				EndCapture();
			}
		}
	}
	void DX11CaptureTask::MessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(20))
			{
				EndCapture();
				break;
			}
			Tick();
		}
	}
	DX11Image&	DX11CaptureTask::GetTexture()
	{
		return m_texture.GetTexture();
	}
	WNDINFO	DX11CaptureTask::GetWNDINFO()
	{
		return m_targetWND;
	}
}
