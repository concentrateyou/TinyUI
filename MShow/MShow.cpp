// MShow.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include "MShow.h"
#include "resource.h"
#include "CrashRpt.h"
#include <math.h>
#include <algorithm>
using namespace std;
#pragma comment(lib,"CrashRpt1403.lib");

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
	__declspec(thread) QWORD g_lastQPCTime = 0;
	//////////////////////////////////////////////////////////////////////////
	MShowApp::MShowApp()
		:m_searchCTRL(m_searchView),
		m_clientCRTL(m_clientView),
		m_audioTS(0)
	{
		timeBeginPeriod(1);
	}
	MShowApp::~MShowApp()
	{
		timeEndPeriod(1);
	}
	BOOL MShowApp::Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName)
	{
		avcodec_register_all();
		QueryPerformanceFrequency(&g_clockFreq);
		LoadSeDebugPrivilege();
		UNREFERENCED_PARAMETER(lpCmdLine);
		UNREFERENCED_PARAMETER(nCmdShow);
		TinyVisualResource::GetInstance().Load("skin\\resource.xml");
		::DefWindowProc(NULL, 0, 0, 0L);
		if (!TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, lpTableName))
			return FALSE;
		if (!TinyApplication::GetInstance()->AddMessageLoop(&m_msgLoop))
			return FALSE;
		string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\search.xml");
		if (!m_searchView.Create(NULL, szFile.c_str()))
			return FALSE;
		m_searchView.ShowWindow(SW_SHOW);
		m_searchView.UpdateWindow();
		szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\client.xml");
		if (!m_clientView.Create(NULL, szFile.c_str()))
			return FALSE;
		m_clientView.ShowWindow(SW_HIDE);
		m_clientView.UpdateWindow();
		if (!m_searchCTRL.Initialize())
			return FALSE;
		if (!m_clientCRTL.Initialize())
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
		m_clientCRTL.Uninitialize();
		m_clientView.DestroyWindow();
		m_searchCTRL.Uninitialize();
		m_searchView.DestroyWindow();
		if (!TinyApplication::GetInstance()->RemoveMessageLoop())
			return FALSE;
		if (!TinyApplication::GetInstance()->Uninitialize())
			return FALSE;
		CloseLogFile();
		return TRUE;
	}

	MSearchWindow& MShowApp::GetSearchView()
	{
		return m_searchView;
	}
	MSearchController& MShowApp::GetSearchController()
	{
		return m_searchCTRL;
	}

	MClientWindow& MShowApp::GetClientView()
	{
		return m_clientView;
	}

	MClientController& MShowApp::GetClientController()
	{
		return m_clientCRTL;
	}

	MShowApp& MShowApp::GetInstance() throw()
	{
		static MShowApp app;
		return app;
	}

	MAppConfig& MShowApp::AppConfig()
	{
		return m_appConfig;
	}

	QWORD MShowApp::GetQPCTimeNS()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
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
		g_lastQPCTime = currentTime.QuadPart;
		DOUBLE timeVal = DOUBLE(currentTime.QuadPart);
		timeVal *= 10000000.0;
		timeVal /= DOUBLE(g_clockFreq.QuadPart);
		return QWORD(timeVal);
	}
	BOOL MShowApp::SleepNS(QWORD qwNSTime)
	{
		QWORD s = GetQPCTimeNS();
		if (s >= qwNSTime)
			return FALSE;
		UINT ms = (UINT)((qwNSTime - s) / 1000000);
		if (ms > 1)
		{
			if (ms > 10000)
				return FALSE;
			Sleep(ms);
		}
		for (;;)
		{
			s = GetQPCTimeNS();
			if (s >= qwNSTime)
				return TRUE;
			Sleep(0);
		}
	}

	void MShowApp::SetCurrentAudioTS(QWORD ts)
	{
		TinyAutoLock lock(m_lock);
		if (ts < m_audioTS)
		{
			LOG(ERROR) << "[SetCurrentAudioTS] AudioTS Error:" << m_audioTS;
		}
		m_audioTS = ts;
	}
	QWORD MShowApp::GetCurrentAudioTS()
	{
		return m_audioTS;
	}
	string MShowApp::GetDefaultPath()
	{
		return m_szPath;
	}
}

BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)
{
	return TRUE;
}

BOOL BuildCrash()
{
	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);
	info.pszAppName = _T("MShow");
	info.pszAppVersion = _T("1.0.0");
	string dir = GetDefaultDumpDir();
	info.pszErrorReportSaveDir = dir.c_str();
	info.pfnCrashCallback = CrashCallback;
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS |
		CR_INST_CRT_EXCEPTION_HANDLERS |
		CR_INST_AUTO_THREAD_HANDLERS |
		CR_INST_SEND_QUEUED_REPORTS;
	info.dwFlags |= CR_INST_DONT_SEND_REPORT;
	info.uMiniDumpType = (MINIDUMP_TYPE)(MiniDumpWithDataSegs |
		MiniDumpWithFullMemory |
		MiniDumpWithHandleData |
		MiniDumpFilterMemory |
		MiniDumpScanMemory |
		MiniDumpWithUnloadedModules |
		MiniDumpWithIndirectlyReferencedMemory |
		MiniDumpFilterModulePaths |
		MiniDumpWithProcessThreadData |
		MiniDumpWithPrivateReadWriteMemory |
		MiniDumpWithoutOptionalData |
		MiniDumpWithFullMemoryInfo |
		MiniDumpWithThreadInfo |
		MiniDumpWithCodeSegs
		);
	INT iResult = crInstall(&info);
	if (iResult != 0)
	{
		TCHAR szErrorMsg[512] = _T("");
		crGetLastErrorMsg(szErrorMsg, 512);
		_tprintf_s(_T("%s\n"), szErrorMsg);
		return FALSE;
	}
	return TRUE;
}

void handlenew()
{
	LOG(ERROR) << "Alloc Memery FAIL";
}

LONG WINAPI OnCrashHandler(LPEXCEPTION_POINTERS ps)
{
	if (ps->ExceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_C)
	{
		StackTrace trace(ps);
		LOG(ERROR) << "[OnCrashHandler]: " << trace.ToString();
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI OnVectoredHandler(struct _EXCEPTION_POINTERS *ps)
{
	if (ps->ExceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_C)
	{
		StackTrace trace(ps);
		LOG(ERROR) << "[OnVectoredHandler]: " << trace.ToString();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

INT APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ INT       nCmdShow)
{
	LOG(INFO) << "LiveCommentary Current Process " << GetCurrentProcessId();

	if (!BuildCrash())
	{
		LOG(ERROR) << "BuildCrash FAIL";
		return FALSE;
	}
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);
	OleInitialize(NULL);
	set_new_handler(handlenew);
	AddVectoredExceptionHandler(1, OnVectoredHandler);
	SetUnhandledExceptionFilter(OnCrashHandler);
	//确保MShowApp先释放
	TinyApplication* app = TinyApplication::GetInstance();

	MShow::MShowApp& showApp = MShow::MShowApp::GetInstance();
	showApp.Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_MSHOW));
	INT iRes = showApp.Run();

	OleUninitialize();
	MFShutdown();
	WSACleanup();
	crUninstall();
	return iRes;
}
