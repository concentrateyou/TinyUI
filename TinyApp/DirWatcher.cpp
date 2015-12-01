#include "stdafx.h"
#include "DirWatcher.h"

WatcherOP::WatcherOP()
	:m_hIocp(nullptr),
	m_hDirectory(nullptr)
{
	this->hEvent = NULL;
	this->Internal = 0;
	this->InternalHigh = 0;
	this->Offset = 0;
	this->OffsetHigh = 0;
	this->Pointer = NULL;
	this->m_dwSize = 0;
	memset(m_pzDirName, 0, MAX_BUFFER);
}

WatcherOP::~WatcherOP()
{

}
//////////////////////////////////////////////////////////////////////////

IOCP::IOCP()
	:m_hFileHandle(NULL),
	m_hIOCP(NULL),
	m_dwConcurrent(0)
{

}
BOOL IOCP::Initialize(DWORD dwConcurrent /* = 1 */)
{
	m_dwConcurrent = dwConcurrent;
	return (m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, m_dwConcurrent)) != NULL;
}
BOOL IOCP::Cancel()
{
	if (m_hIOCP != NULL)
	{
		if (FARPROC CancelIoExPtr = ::GetProcAddress(
			::GetModuleHandleA("KERNEL32"), "CancelIoEx"))
		{
			CancelIoEx cancelIoEx = (CancelIoEx)CancelIoExPtr;
			return cancelIoEx(m_hIOCP, NULL);
		}
	}
	return FALSE;
}
void IOCP::Close()
{
	if (m_hIOCP != NULL)
	{
		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
	}
}
IOCP::~IOCP()
{
	Close();
}
IOCP::operator HANDLE() const
{
	return m_hIOCP;
}
HANDLE	IOCP::Handle() const
{
	return m_hIOCP;
}
BOOL IOCP::Register(HANDLE hFileHandle, ULONG_PTR completionKey)
{
	if (m_hIOCP && hFileHandle)
	{
		if (hFileHandle != m_hFileHandle)
		{
			m_hFileHandle = hFileHandle;
			return ::CreateIoCompletionPort(hFileHandle, m_hIOCP, completionKey, 0) != NULL;
		}
		return FALSE;
	}
	return FALSE;
}
HANDLE	IOCP::GetFileHandle() const
{
	return m_hFileHandle;
}
ULONG_PTR IOCP::GetCompletionKey() const
{
	return m_completionKey;
}
//////////////////////////////////////////////////////////////////////////
FolderWatcher::FolderWatcher()
{

}

FolderWatcher::~FolderWatcher()
{
	Uninitialize();
}

BOOL FolderWatcher::Initialize()
{
	GetSystemInfo(&m_systemInfo);
	m_systemInfo.dwNumberOfProcessors = 3;//调试
	if (m_iocp.Initialize(m_systemInfo.dwNumberOfProcessors))
	{
		for (DWORD i = 0; i < m_systemInfo.dwNumberOfProcessors; i++)
		{
			HANDLE hThread = CreateThread(NULL, 0, FolderWatcher::doWatchTask, this, 0, NULL);
			if (hThread != NULL)
			{
				CloseHandle(hThread);
			}
		}
		return TRUE;
	}
	return FALSE;
}
void FolderWatcher::Uninitialize()
{
	if (m_iocp)
	{
		/*WatcherOP* op = new WatcherOP();
		op->m_hIocp = m_iocp;
		op->m_op = OP::CLOSE_WATCH;
		PostQueuedCompletionStatus(m_iocp, sizeof(this), (DWORD)this, op);*/
	}
}
std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

DWORD WINAPI FolderWatcher::doWatchTask(void* ps)
{
	FolderWatcher* watcher = reinterpret_cast<FolderWatcher*>(ps);
	DWORD dwNumberOfBytesTransferred = 0;
	LPOVERLAPPED lpOverlapped = NULL;
	WatcherOP* op = NULL;
	do
	{
		if (GetQueuedCompletionStatus(watcher->Handle(),
			&dwNumberOfBytesTransferred,
			(LPDWORD)&op,
			&lpOverlapped,
			INFINITE) && op)
		{
			TRACE("GetQueuedCompletionStatus OK\n");
			PFILE_NOTIFY_INFORMATION notify = (PFILE_NOTIFY_INFORMATION)op->m_data;
			DWORD dwOffSet = notify->NextEntryOffset;
			do
			{
				switch (notify->Action)
				{
				case FILE_ACTION_ADDED:
				{
					wstring str(notify->FileName, notify->FileNameLength / sizeof(WCHAR));
					string str1 = ws2s(str);
					str1 = string(op->m_pzDirName) + "\\" + str1;
					TRACE("FILE_ACTION_ADDED: %s\n", str1.c_str());
				}
				break;
				case FILE_ACTION_REMOVED:
				{
					wstring str(notify->FileName, notify->FileNameLength / sizeof(WCHAR));
					string str1 = ws2s(str);
					str1 = string(op->m_pzDirName) + "\\" + str1;
					TRACE("FILE_ACTION_REMOVED: %s\n", str1.c_str());
				}
				break;
				}
				dwOffSet = notify->NextEntryOffset;
				notify = (PFILE_NOTIFY_INFORMATION)((LPBYTE)notify + dwOffSet);
			} while (dwOffSet);
			ReadDirectoryChangesW(op->m_hDirectory, op->m_data,
				MAX_BUFFER,
				TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME,
				&op->m_dwSize,
				op,
				NULL);
		}
	} while (op);

	return 0;
}

HANDLE FolderWatcher::GetHandleByFile(string s)
{
	for (size_t i = 0; i < m_watcherOps.size(); i++)
	{
		WatcherOP* op = m_watcherOps[i];
		string str(op->m_data);
		if (str == s)
		{
			return op->m_hDirectory;
		}
	}
	return nullptr;
}
void FolderWatcher::RemoveHandleByFile(string s)
{
	for (size_t i = m_watcherOps.size() - 1; i >= 0; i--)
	{
		WatcherOP* op = m_watcherOps[i];
		string str(op->m_data);
		if (str == s)
		{
			//m_watcherOps.erase(op);
			break;
		}
	}
}
void FolderWatcher::PostOP(HANDLE hDirectory, string name, OP op1)
{
	if (m_iocp)
	{
		WatcherOP* op = new WatcherOP();
		op->m_hIocp = m_iocp;
		op->m_hDirectory = hDirectory;
		op->m_dwSize = name.size();
		op->m_op = op1;
		PostQueuedCompletionStatus(m_iocp, 0, 0, op);
	}
}
BOOL FolderWatcher::Watch(string str)
{
	//创建监视的文件句柄
	HANDLE hDirectory = CreateFile(str.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if (hDirectory == INVALID_HANDLE_VALUE) return FALSE;
	WatcherOP* op = new WatcherOP();
	op->m_hDirectory = hDirectory;
	memcpy(op->m_pzDirName, str.c_str(), str.size());
	m_watcherOps.push_back(op);
	if (m_iocp.Register(hDirectory, reinterpret_cast<ULONG_PTR>(op)))
	{
		return ReadDirectoryChangesW(op->m_hDirectory,
			op->m_data,
			MAX_BUFFER,
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME,
			&op->m_dwSize,
			op,
			NULL);
	}
	return FALSE;
}

void FolderWatcher::Unwatch(string str)
{
	if (m_iocp)
	{
		//RemoveHandleByFile(str);
	}
}
HANDLE  FolderWatcher::Handle() const
{
	return m_iocp;
}

