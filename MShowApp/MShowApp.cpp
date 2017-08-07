// MShowApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MShowApp.h"
#include "RTMPReader.h"
#include "resource.h"
#include <math.h>
#include <algorithm>
using namespace std;

namespace MShow
{
	BOOL LoadSeDebugPrivilege()
	{
		DWORD   err;
		HANDLE  hToken;
		LUID    Val;
		TOKEN_PRIVILEGES tp;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			err = GetLastError();
			return FALSE;
		}
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
		{
			err = GetLastError();
			CloseHandle(hToken);
			return FALSE;
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = Val;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
		{
			err = GetLastError();
			CloseHandle(hToken);
			return FALSE;
		}
		CloseHandle(hToken);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	LARGE_INTEGER g_clockFreq;
	__declspec(thread) LONGLONG g_lastQPCTime = 0;
	//////////////////////////////////////////////////////////////////////////
	MShowApp::MShowApp()
		:m_controller(m_window)
	{
		timeBeginPeriod(1);
	}
	MShowApp::~MShowApp()
	{
		timeEndPeriod(1);
	}
	BOOL MShowApp::Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName)
	{
		QueryPerformanceFrequency(&g_clockFreq);
		LoadSeDebugPrivilege();
		UNREFERENCED_PARAMETER(lpCmdLine);
		UNREFERENCED_PARAMETER(nCmdShow);
		::DefWindowProc(NULL, 0, 0, 0L);
		if (!TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, lpTableName))
			return FALSE;
		if (!TinyApplication::GetInstance()->AddMessageLoop(&m_msgLoop))
			return FALSE;
		if (!m_window.Create(NULL, 0, 0, 1, 1))
			return FALSE;
		if (!m_controller.Initialize())
			return FALSE;
		return TRUE;
	}
	INT MShowApp::Run()
	{
		INT iRes = m_msgLoop.MessageLoop();
		this->Uninitialize();
		return iRes;
	}
	BOOL MShowApp::Uninitialize()
	{
		m_controller.Uninitialize();
		if (!TinyApplication::GetInstance()->RemoveMessageLoop())
			return FALSE;
		if (!TinyApplication::GetInstance()->Uninitialize())
			return FALSE;
		return TRUE;
	}

	MShowWindow& MShowApp::GetView()
	{
		return m_window;
	}
	MShowController& MShowApp::GetController()
	{
		return m_controller;
	}

	MShowApp& MShowApp::GetInstance() throw()
	{
		static MShowApp app;
		return app;
	}
	QWORD MShowApp::GetQPCTimeNS()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		ASSERT(currentTime.QuadPart >= g_lastQPCTime);
		g_lastQPCTime = currentTime.QuadPart;
		DOUBLE timeVal = DOUBLE(currentTime.QuadPart);
		timeVal *= 1000000000.0;
		timeVal /= DOUBLE(g_clockFreq.QuadPart);
		return QWORD(timeVal);
	}

	QWORD MShowApp::GetQPCTimeMS()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		ASSERT(currentTime.QuadPart >= g_lastQPCTime);
		g_lastQPCTime = currentTime.QuadPart;
		QWORD timeVal = currentTime.QuadPart;
		timeVal *= 1000;
		timeVal /= g_clockFreq.QuadPart;
		return timeVal;
	}

	QWORD MShowApp::GetQPCTime100NS()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		ASSERT(currentTime.QuadPart >= g_lastQPCTime);
		g_lastQPCTime = currentTime.QuadPart;
		DOUBLE timeVal = DOUBLE(currentTime.QuadPart);
		timeVal *= 10000000.0;
		timeVal /= DOUBLE(g_clockFreq.QuadPart);
		return QWORD(timeVal);
	}
}

//////////////////////////////////////////////////////////////////////////
INT APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);
	OleInitialize(NULL);
	MShow::MShowApp& app = MShow::MShowApp::GetInstance();
	app.Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_MSHOWAPP));
	INT iRes = app.Run();
	OleUninitialize();
	MFShutdown();
	WSACleanup();
	return iRes;
}
