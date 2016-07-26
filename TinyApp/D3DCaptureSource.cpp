#include "stdafx.h"
#include "D3DCaptureSource.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

CD3DCaptureSource::CD3DCaptureSource()
	:m_hGameHWND(NULL),
	m_bCapturing(FALSE),
	m_hGameProcess(NULL)
{
}


CD3DCaptureSource::~CD3DCaptureSource()
{
}

BOOL CD3DCaptureSource::Initialize(const TinyString& processName)
{
	BOOL bRes = TRUE;
	stringstream stream;
	DWORD dwProcessID = FindProcess(processName);
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (!hProcess)
	{
		bRes = FALSE;
		goto D3DERROR;
	}
	if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_hGameProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		bRes = FALSE;
		goto D3DERROR;
	}
	stream << BEGIN_CAPTURE_EVENT << dwProcessID;
	if (!m_beginCapture.CreateEvent(FALSE, FALSE, stream.str().c_str()))
	{
		if (!m_beginCapture.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
		{
			bRes = FALSE;
			goto D3DERROR;
		}
	}
	stream.clear();
	stream << END_CAPTURE_EVENT << dwProcessID;
	if (!m_endCapture.CreateEvent(FALSE, FALSE, stream.str().c_str()))
	{
		if (!m_endCapture.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
		{
			bRes = FALSE;
			goto D3DERROR;
		}
	}
	m_hGameHWND = GetGameWindow(dwProcessID);
	if (!m_hGameHWND)
	{
		bRes = FALSE;
		goto D3DERROR;
	}
D3DERROR:
	if (hProcess)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}
	return bRes;
}
BOOL CD3DCaptureSource::BeginCapture()
{
	if (!D3D::InjectLibrary(m_hGameProcess, TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\D3DCaptureHook.dll")))
		return FALSE;
	m_bCapturing = TRUE;
	m_beginCapture.SetEvent();
	return TRUE;
}

BOOL CD3DCaptureSource::EndCapture()
{
	return FALSE;
}

BOOL CALLBACK CD3DCaptureSource::CaptureFindWindow(HWND hwnd, LPARAM lParam)
{
	LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
	DWORD dwProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	if (dwProcessId == ws->dwProcessID)
	{
		ws->hWND = hwnd;
		HWND hWND = ::GetParent(hwnd);
		while (hWND)
		{
			ws->hWND = hWND;
			hWND = ::GetParent(hWND);
		}
		return FALSE;
	}
	return TRUE;
}

HWND CD3DCaptureSource::GetGameWindow(DWORD dwProcessID)
{
	WNDINFO ws;
	ws.dwProcessID = dwProcessID;
	EnumWindows(CD3DCaptureSource::CaptureFindWindow, reinterpret_cast<LPARAM>(&ws));
	return ws.hWND;
}

DWORD CD3DCaptureSource::FindProcess(const TinyString& processName)
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

