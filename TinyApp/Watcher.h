#pragma once
#include <Windows.h>
#include <Shellapi.h>
#include <Commdlg.h>
#include <comdef.h>
#include <Mmsystem.h>
#include <new.h>
#include <WinGDI.h>
#include <GdiPlus.h>
#include <Shlwapi.h> 
#include <commctrl.inl>
#include <Psapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <crtdbg.h>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include "SkinFramework/TinySkinWindow.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Comctl32.lib")

using namespace TinyUI;
using namespace std;

enum OP
{
	START_WATCH = 0,
	WATCHING = 1,
	STOP_WATCH = 2,
	CLOSE_WATCH = 3
};
#define MAX_BUFFER 4096
class FolderWatcher;
//监控操作类
class WatcherOP : public OVERLAPPED
{
public:
	WatcherOP();
	~WatcherOP();
public:
	OP              m_op;
	HANDLE          m_hIocp;
	HANDLE          m_hDirectory;
	CHAR			m_pzDirName[MAX_PATH];
	CHAR			m_data[MAX_BUFFER];
	DWORD			m_dwSize;//换从大小
};

class IOCP
{
public:
	IOCP();
	~IOCP();
	operator HANDLE() const;
	HANDLE		Handle() const;
	BOOL        Initialize(DWORD dwConcurrent = 1);
	BOOL		Register(HANDLE hFileHandle, ULONG_PTR completionKey);
	BOOL		Cancel();
	void		Close();
	HANDLE		GetFileHandle() const;
	ULONG_PTR	GetCompletionKey() const;
public:
	typedef BOOL(WINAPI* CancelIoEx)(HANDLE, LPOVERLAPPED);
private:
	HANDLE							m_hIOCP;
	DWORD							m_dwConcurrent;
	ULONG_PTR						m_completionKey;
	HANDLE							m_hFileHandle;
};

class FolderWatcher
{

public:
	FolderWatcher();
	~FolderWatcher();
	HANDLE  Handle() const;
	BOOL Initialize();
	void Uninitialize();
	BOOL Watch(string str);
	void Unwatch(string str);
	void PostOP(HANDLE hDirectory, string name, OP op1);
	static DWORD WINAPI doWatchTask(void* ps);
private:
	HANDLE  GetHandleByFile(string file);
	void    RemoveHandleByFile(string file);
private:
	SYSTEM_INFO m_systemInfo;
	IOCP m_iocp;
	vector<WatcherOP*>	m_watcherOps;
};

