#include "stdafx.h"
#include "D3DCaptureSource.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

namespace D3D
{
	CD3DCaptureSource::CD3DCaptureSource(CD3DSystem& system)
		:m_bCapturing(FALSE),
		m_sharedTexture(system)
	{
		::ZeroMemory(&m_targetWND, sizeof(m_targetWND));
	}


	CD3DCaptureSource::~CD3DCaptureSource()
	{
		if (m_targetWND.hProcess)
		{
			CloseHandle(m_targetWND.hProcess);
		}
	}

	BOOL CD3DCaptureSource::Initialize(const TinyString& processName)
	{
		if (!FindWindow(processName))
			return FALSE;
		stringstream stream;
		stream << BEGIN_CAPTURE_EVENT << m_targetWND.dwProcessID;
		if (!m_eventBegin.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventBegin.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		stream.str("");
		stream << END_CAPTURE_EVENT << m_targetWND.dwProcessID;
		if (!m_eventEnd.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventEnd.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		stream.str("");
		stream << CAPTURE_READY_EVENT << m_targetWND.dwProcessID;
		if (!m_eventReady.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL CD3DCaptureSource::BeginCapture(const TinyString& processName)
	{
		BOOL bRes = S_OK;
		if (!Initialize(processName))
			return FALSE;
		if (!D3D::InjectLibrary(m_targetWND.hProcess, TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\D3DCaptureHook.dll")))
			return FALSE;
		SharedCapture* sharedCapture = GetSharedCapture();
		if (!sharedCapture)
			return FALSE;
		if (!m_sharedTexture.Initialize())
			return FALSE;
		m_bCapturing = TRUE;
		m_eventBegin.SetEvent();
		return TRUE;
	}

	BOOL CD3DCaptureSource::EndCapture()
	{
		m_bCapturing = FALSE;
		m_eventEnd.SetEvent();
		return TRUE;
	}

	SharedCapture* CD3DCaptureSource::GetSharedCapture()
	{
		if (m_sharedCapture.Open(SHAREDCAPTURE) && m_sharedCapture.Map())
		{
			return reinterpret_cast<SharedCapture*>(m_sharedCapture.Address());
		}
		return NULL;
	}
	void CD3DCaptureSource::Tick(FLOAT fSeconds)
	{
		if (m_eventExit.Lock(0))
		{
			EndCapture();
		}
		if (!m_bCapturing)
		{
			BOOL hRes = BeginCapture(TEXT("War3.exe"));
		}
		else
		{

		}
	}
	void CD3DCaptureSource::Render(const POINT &pos, const SIZE &size)
	{

	}

	BOOL CALLBACK CD3DCaptureSource::EnumWindow(HWND hwnd, LPARAM lParam)
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
	BOOL CD3DCaptureSource::FindWindow(const TinyString& processName)
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
		bRes = EnumWindows(CD3DCaptureSource::EnumWindow, reinterpret_cast<LPARAM>(&ws));
		m_targetWND.dwThreadID = ws.dwThreadID;
		m_targetWND.hWND = ws.hWND;
	D3DERROR:
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return bRes;
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
}
