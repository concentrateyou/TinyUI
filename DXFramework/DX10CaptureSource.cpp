#include "stdafx.h"
#include "DX10CaptureSource.h"

namespace DXFramework
{
	DX10CaptureSource::DX10CaptureSource()
	{
	}
	DX10CaptureSource::~DX10CaptureSource()
	{
	}
	BOOL DX10CaptureSource::Initialize(const TinyString& processName)
	{
		if (!FindWindow(processName))
			return FALSE;
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_eventBegin.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_eventBegin.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_eventEnd.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_eventEnd.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID);
		if (!m_eventReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_eventReady.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", APP_EXIT_EVENT, m_targetWND.dwProcessID);
		if (!m_eventExit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_eventExit.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	DWORD DX10CaptureSource::FindProcess(const TinyString& processName)
	{
		DWORD dwProcessID = 0;
		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = { 0 };
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == (HANDLE)-1)
			return FALSE;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				TinyString szExeFile(pe32.szExeFile);
				if (szExeFile == processName)
				{
					dwProcessID = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return dwProcessID;
	}
	BOOL DX10CaptureSource::FindWindow(const TinyString& processName)
	{
		if (m_targetWND.hProcess)
			CloseHandle(m_targetWND.hProcess);
		BOOL bRes = S_OK;
		m_targetWND.dwProcessID = FindProcess(processName);
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID);
		if (!hProcess)
		{
			bRes = FALSE;
			goto D3DERROR;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_targetWND.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			bRes = FALSE;
			goto D3DERROR;
		}
		WNDINFO ws;
		ws.dwProcessID = m_targetWND.dwProcessID;
		bRes = EnumWindows(DX10CaptureSource::EnumWindow, reinterpret_cast<LPARAM>(&ws));
		m_targetWND.dwThreadID = ws.dwThreadID;
		m_targetWND.hWND = ws.hWND;
	D3DERROR:
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return bRes == S_OK;
	}
	SharedCapture* DX10CaptureSource::GetSharedCapture()
	{
		if (m_sharedMemory.Open(SHAREDCAPTURE_MEMORY) && m_sharedMemory.Map())
		{
			return reinterpret_cast<SharedCapture*>(m_sharedMemory.Address());
		}
		return NULL;
	}
	BOOL CALLBACK DX10CaptureSource::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		DWORD dwProcessId;
		DWORD dwThreadID = GetWindowThreadProcessId(hwnd, &dwProcessId);
		if (dwProcessId == ws->dwProcessID)
		{
			ws->hWND = hwnd;
			HWND hWND = ::GetParent(hwnd);
			while (hWND)
			{
				ws->hWND = hWND;
				ws->dwThreadID = dwThreadID;
				hWND = ::GetParent(hWND);
			}
			return FALSE;
		}
		return TRUE;
	}
}
