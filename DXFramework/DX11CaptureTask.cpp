#include "stdafx.h"
#include "DX11CaptureTask.h"

namespace DXFramework
{
	DX11CaptureTask::DX11CaptureTask(DX11* pDX11, TinyTaskPool* pWorks)
		:TinyTask(pWorks),
		m_bCapturing(FALSE),
		m_pDX11(pDX11)
	{
		ZeroMemory(&m_targetWND, sizeof(m_targetWND));

	}
	DX11CaptureTask::~DX11CaptureTask()
	{
	}
	BOOL DX11CaptureTask::Submit()
	{
		Closure s = BindCallback(&DX11CaptureTask::MessagePump, this);
		return TinyTask::Submit(s);
	}
	DWORD DX11CaptureTask::BuildEvents()
	{
		if (!m_start)
		{
			string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
			if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_stop)
		{
			string name = StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID);
			if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_ready)
		{
			string name = StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID);
			if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_exit)
		{
			string name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID);
			if (!m_exit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()) &&
				!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		if (!m_close)
		{
			if (!m_close.OpenEvent(EVENT_ALL_ACCESS, FALSE, WINDOW_CLOSE_EVENT) &&
				!m_close.CreateEvent(FALSE, FALSE, WINDOW_CLOSE_EVENT))
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
		if (!m_texture.Initialize(*m_pDX11, 600, 400))
			return FALSE;
		return TRUE;
	}
	BOOL DX11CaptureTask::EndCapture()
	{
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
		m_start.SetEvent();
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
		if (m_exit && m_exit.Lock(0))
		{
			EndCapture();
		}
		if (m_bCapturing && !m_ready && m_targetWND.dwProcessID)
		{
			BuildEvents();
		}
		if (m_ready && m_ready.Lock(0))
		{
			BeginCapture();
		}
		if (!m_bCapturing)
		{
			AttemptCapture(TEXT("Warcraft III"), TEXT("war3.exe"), TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\GameDetour.dll"));
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
			Tick();
			if (m_close && m_close.Lock(0))
			{
				EndCapture();
				return;
			}
			Sleep(1);
		}
	}
	DX11Image*	DX11CaptureTask::GetTexture()
	{
		return m_texture.GetTexture();
	}
	WNDINFO	DX11CaptureTask::GetWNDINFO()
	{
		return m_targetWND;
	}
}
