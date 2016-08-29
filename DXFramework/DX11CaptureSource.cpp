#include "stdafx.h"
#include "DX11CaptureSource.h"

namespace DXFramework
{
	DX11CaptureSource::DX11CaptureSource()
	{
	}
	DX11CaptureSource::~DX11CaptureSource()
	{
	}
	BOOL DX11CaptureSource::Initialize(const TinyString& processName)
	{
		if (!FindWindow(processName))
			return FALSE;
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_begin.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_begin.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetWND.dwProcessID);
		if (!m_end.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_end.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetWND.dwProcessID);
		if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetWND.dwProcessID);
		if (!m_exit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			if (!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	DWORD DX11CaptureSource::FindProcess(const TinyString& processName)
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
	BOOL DX11CaptureSource::FindWindow(const TinyString& processName)
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
		bRes = EnumWindows(DX11CaptureSource::EnumWindow, reinterpret_cast<LPARAM>(&ws));
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
	SharedCapture* DX11CaptureSource::GetSharedCapture()
	{
		if (m_memory.Open(SHAREDCAPTURE_MEMORY) && m_memory.Map())
		{
			return reinterpret_cast<SharedCapture*>(m_memory.Address());
		}
		return NULL;
	}
	BOOL CALLBACK DX11CaptureSource::EnumWindow(HWND hwnd, LPARAM lParam)
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
	BOOL DX11CaptureSource::BeginCapture(const DX11& dx10, const TinyString& processName, const TinyString& dll)
	{
		BOOL bRes = S_OK;
		if (!Initialize(processName))
			return FALSE;
		if (!m_bInject)
		{
			m_bInject = InjectLibrary(m_targetWND.hProcess, "D:\\Develop\\GitHub\\TinyUI\\Debug\\GameDetour.dll");
			if (!m_bInject)
				return FALSE;
		}
		if (m_ready.Lock(0))
		{
			SharedCapture* ps = GetSharedCapture();
			if (!ps)
				return FALSE;
			if (!m_textureCapture.Initialize(dx10, ps))
				return FALSE;
			m_bCapturing = TRUE;
		}
		m_begin.SetEvent();
		return TRUE;
	}
	BOOL DX11CaptureSource::EndCapture()
	{
		m_bCapturing = FALSE;
		m_end.SetEvent();
		return TRUE;
	}
	void DX11CaptureSource::Tick(const DX11& dx10)
	{
		if (m_exit && m_exit.Lock(0))
		{
			EndCapture();
		}
		if (!m_bCapturing)
		{
			BeginCapture(dx10, TEXT("War3.exe"), TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\GameDetour.dll"));
		}
		Sleep(1);
	}
}
