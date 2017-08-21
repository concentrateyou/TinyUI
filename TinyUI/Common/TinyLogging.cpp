#include "../stdafx.h"
#include "TinyLogging.h"
#include "../IO/TinyIO.h"
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <ostream>
#include <string>

namespace TinyUI
{
	IO::TinyFile g_logFile;
	string g_log;

	string GetDefaultLogFile()
	{
		CHAR module[MAX_PATH];
		GetModuleFileName(NULL, module, MAX_PATH);
		string log(module);
		string::size_type backslash = log.rfind('\\', log.size());
		if (backslash != string::npos)
		{
			log.erase(backslash + 1);
		}
		log += TEXT("debug.log");
		return log;
	}
	wstring GetDefaultLogFileW()
	{
		WCHAR module[MAX_PATH];
		GetModuleFileNameW(NULL, module, MAX_PATH);
		wstring log(module);
		wstring::size_type backslash = log.rfind('\\', log.size());
		if (backslash != wstring::npos)
		{
			log.erase(backslash + 1);
		}
		log += TEXT(L"debug.log");
		return log;
	}
	BOOL InitializeLogFile(LPCSTR pzFile)
	{
		if (g_logFile != INVALID_HANDLE_VALUE || g_logFile != NULL)
			return TRUE;
		g_log = (pzFile == NULL ? GetDefaultLogFile() : pzFile);
		if (!g_logFile.Open(g_log.c_str(), FILE_APPEND_DATA, FILE_ATTRIBUTE_NORMAL))
		{
			CHAR directory[MAX_PATH];
			directory[0] = 0;
			DWORD s = ::GetCurrentDirectory(arraysize(directory), directory);
			if (s == 0 || s > arraysize(directory))
				return FALSE;
			g_log = directory;
			if (g_log.back() != TEXT('\\'))
				g_log += TEXT("\\");
			g_log += TEXT("debug.log");
			if (!g_logFile.Open(g_log.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_ATTRIBUTE_NORMAL))
				return FALSE;
		}
		return TRUE;
	}
	BOOL CloseLogFile()
	{
		return g_logFile.Close();
	}
	BOOL SetLogFile(LPCSTR pzFile)
	{
		g_log = pzFile;
		return InitializeLogFile(pzFile);
	}
	//////////////////////////////////////////////////////////////////////////
	SymbolContext::SymbolContext() : m_error(ERROR_SUCCESS)
	{
		SymSetOptions(SYMOPT_DEFERRED_LOADS |
			SYMOPT_UNDNAME |
			SYMOPT_LOAD_LINES);
		if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
		{
			m_error = GetLastError();
			return;
		}
		const size_t symbolsArraySize = 1024;
		TinyScopedArray<wchar_t> symbols_path(new wchar_t[symbolsArraySize]);
		if (!SymGetSearchPathW(GetCurrentProcess(), (PWSTR)symbols_path.Ptr(), symbolsArraySize))
		{
			m_error = GetLastError();
			return;
		}
	}
	void SymbolContext::OutputTraceToStream(const void* const* trace, size_t count, std::ostream* os)
	{
		TinyAutoLock lock(m_lock);
		for (size_t i = 0; (i < count) && os->good(); ++i)
		{
			const int maxNameLength = 256;
			DWORD_PTR frame = reinterpret_cast<DWORD_PTR>(trace[i]);
			// http://msdn.microsoft.com/en-us/library/ms680578(VS.85).aspx
			ULONG64 buffer[
				(sizeof(SYMBOL_INFO) +
					maxNameLength * sizeof(wchar_t) +
					sizeof(ULONG64) - 1) /
					sizeof(ULONG64)];
			memset(buffer, 0, sizeof(buffer));
			DWORD64 symDisplacement = 0;
			PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&buffer[0]);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = maxNameLength - 1;
			BOOL hasSymbol = SymFromAddr(GetCurrentProcess(), frame, &symDisplacement, symbol);
			DWORD lineDisplacement = 0;
			IMAGEHLP_LINE64 line = {};
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			BOOL hasLine = SymGetLineFromAddr64(GetCurrentProcess(), frame, &lineDisplacement, &line);
			(*os) << "\t";
			if (hasSymbol)
			{
				(*os) << symbol->Name << " [0x" << trace[i] << "+" << symDisplacement << "]";
			}
			else
			{
				(*os) << "(No symbol) [0x" << trace[i] << "]";
			}
			if (hasLine)
			{
				(*os) << " (" << line.FileName << ":" << line.LineNumber << ")";
			}
			(*os) << "\n";
		}
	}
	DWORD SymbolContext::GetError() const
	{
		return m_error;
	}
	SymbolContext* SymbolContext::GetInstance()
	{
		if (NULL == m_pInstance)
		{
			TinyLock lock;
			lock.Lock();
			m_pInstance = new SymbolContext();
			lock.Unlock();
		}
		return m_pInstance;
	}
	//////////////////////////////////////////////////////////////////////////
	StackTrace::StackTrace()
	{
		m_count = CaptureStackBackTrace(0, arraysize(m_trace), m_trace, NULL);
	}
	StackTrace::StackTrace(const void* const* trace, size_t count)
	{
		count = min(count, arraysize(m_trace));
		if (count)
		{
			memcpy(m_trace, trace, count * sizeof(m_trace[0]));
		}
		m_count = count;
	}
	StackTrace::~StackTrace()
	{
	}
	const void *const *StackTrace::Addresses(size_t* count) const
	{
		*count = m_count;
		if (m_count)
			return m_trace;
		return NULL;
	}
	std::string StackTrace::ToString() const
	{
		std::stringstream stream;
		OutputToStream(&stream);
		return stream.str();
	}
	StackTrace::StackTrace(const EXCEPTION_POINTERS* exception_pointers)
	{
		m_count = 0;
		CONTEXT context_record = *exception_pointers->ContextRecord;
		STACKFRAME64 stack_frame;
		memset(&stack_frame, 0, sizeof(stack_frame));
#if defined(_WIN64)
		int machine_type = IMAGE_FILE_MACHINE_AMD64;
		stack_frame.AddrPC.Offset = context_record.Rip;
		stack_frame.AddrFrame.Offset = context_record.Rbp;
		stack_frame.AddrStack.Offset = context_record.Rsp;
#else
		int machine_type = IMAGE_FILE_MACHINE_I386;
		stack_frame.AddrPC.Offset = context_record.Eip;
		stack_frame.AddrFrame.Offset = context_record.Ebp;
		stack_frame.AddrStack.Offset = context_record.Esp;
#endif
		stack_frame.AddrPC.Mode = AddrModeFlat;
		stack_frame.AddrFrame.Mode = AddrModeFlat;
		stack_frame.AddrStack.Mode = AddrModeFlat;
		while (StackWalk64(machine_type,
			GetCurrentProcess(),
			GetCurrentThread(),
			&stack_frame,
			&context_record,
			NULL,
			&SymFunctionTableAccess64,
			&SymGetModuleBase64,
			NULL) &&
			m_count < arraysize(m_trace))
		{
			m_trace[m_count++] = reinterpret_cast<void*>(stack_frame.AddrPC.Offset);
		}
		for (size_t i = m_count; i < arraysize(m_trace); ++i)
		{
			m_trace[i] = NULL;
		}
	}
	void StackTrace::Print() const
	{
		OutputToStream(&std::cerr);
	}
	void StackTrace::OutputToStream(std::ostream* os) const
	{
		SymbolContext* context = SymbolContext::GetInstance();
		DWORD error = context->GetError();
		if (error != ERROR_SUCCESS)
		{
			(*os) << "Error initializing symbols (" << error << ").  Dumping unresolved backtrace:\n";
			for (size_t i = 0; (i < m_count) && os->good(); ++i)
			{
				(*os) << "\t" << m_trace[i] << "\n";
			}
		}
		else
		{
			(*os) << "Backtrace:\n";
			context->OutputTraceToStream(m_trace, m_count, os);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LogException::LogException(LPCSTR pFile, INT line)
		:m_pFileName(pFile),
		m_line(line),
		m_hFile(NULL)
	{
		Initialize(pFile, line);
	}
	BOOL LogException::Initialize(LPCSTR pFileName, INT line)
	{
		std::string name(pFileName);
		m_stream << '[';
		m_stream << GetCurrentProcessId() << ':';
		m_stream << GetCurrentThreadId() << ':';
		time_t t = time(NULL);
		struct tm local_time = { 0 };
#if _MSC_VER >= 1400
		localtime_s(&local_time, &t);
#else
		localtime_r(&t, &local_time);
#endif
		struct tm* tm_time = &local_time;
		m_stream << std::setfill('0')
			<< std::setw(2) << 1 + tm_time->tm_mon
			<< std::setw(2) << tm_time->tm_mday
			<< '/'
			<< std::setw(2) << tm_time->tm_hour
			<< std::setw(2) << tm_time->tm_min
			<< std::setw(2) << tm_time->tm_sec
			<< ':';
		m_stream << GetTickCount() << ':';
		m_stream << ":" << name << "(" << line << ")] ";
		m_messageOffset = static_cast<size_t>(m_stream.tellp());
		string logFile = GetDefaultLogFile();
		m_hFile = CreateFile(logFile.c_str(), GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
		{
#if _DEBUG
			m_hFile = CreateFile(TEXT(".\\debug.log"), GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
			m_hFile = CreateFile(TEXT(".\\debug.dmp"), GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
			if (!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
			{
				m_hFile = NULL;
				return FALSE;
			}
		}
		SetFilePointer(m_hFile, 0, 0, FILE_END);
		return TRUE;
	}
	void LogException::WriteLog(PEXCEPTION_POINTERS ps)
	{
#if _DEBUG
		StackTrace trace(ps);
		m_stream << std::endl;
		trace.OutputToStream(&m_stream);
		m_stream << std::endl;
		string newline(m_stream.str());
		SetFilePointer(m_hFile, 0, 0, SEEK_END);
		DWORD num_written;
		WriteFile(m_hFile,
			static_cast<const void*>(newline.c_str()),
			static_cast<DWORD>(newline.length()),
			&num_written,
			NULL);
#else
		MINIDUMP_EXCEPTION_INFORMATION mei;
		mei.ThreadId = GetCurrentThreadId();
		mei.ExceptionPointers = ps;
		mei.ClientPointers = FALSE;
		MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory |
			MiniDumpWithDataSegs |
			MiniDumpWithHandleData |
			MiniDumpWithFullMemoryInfo |
			MiniDumpWithThreadInfo |
			MiniDumpWithUnloadedModules);
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), m_hFile, mdt, &mei, NULL, NULL);
#endif
	}
	LogException::~LogException()
	{
		if (m_hFile &&
			m_hFile != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(m_hFile);
			CloseHandle(m_hFile);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const CHAR* const LogSeverityNames[LOG_NUM_SEVERITIES] =
	{
		"INFO", "WARNING", "ERROR", "FATAL" };

	const CHAR* LogSeverityName(INT severity)
	{
		if (severity >= 0 && severity < LOG_NUM_SEVERITIES)
			return LogSeverityNames[severity];
		return "UNKNOWN";
	}
	LogMessage::LogMessage(LPCSTR pzFile, INT line, LogSeverity severity)
		:m_severity(severity),
		m_line(line)
	{
		Initialize(pzFile, line);
	}
	ostream& LogMessage::stream()
	{
		return m_stream;
	}
	void LogMessage::Initialize(LPCSTR pzFile, INT line)
	{
		string szFile(pzFile);
		size_t pos = szFile.find_last_of("\\/");
		if (pos != string::npos)
		{
			szFile.substr(0, pos + 1);
		}
		m_stream << '[';
		m_stream << GetCurrentProcessId() << ':';
		m_stream << GetCurrentThreadId() << ':';
		time_t t = time(NULL);
		struct tm local_time = { 0 };
		localtime_s(&local_time, &t);
		struct tm* tm_time = &local_time;
		m_stream << std::setfill('0')
			<< std::setw(2) << 1 + tm_time->tm_mon
			<< std::setw(2) << tm_time->tm_mday
			<< '/'
			<< std::setw(2) << tm_time->tm_hour
			<< std::setw(2) << tm_time->tm_min
			<< std::setw(2) << tm_time->tm_sec
			<< ':';
		if (m_severity >= 0)
		{
			m_stream << LogSeverityName(m_severity);
		}
		m_stream << ":" << szFile << "(" << line << ")] ";
	}
	LogMessage::~LogMessage()
	{
		m_stream << std::endl;
		string newline(m_stream.str());
		if (InitializeLogFile(g_log.c_str()))
		{
			g_logFile.Write(static_cast<const void*>(newline.c_str()), static_cast<DWORD>(newline.length()));
			g_logFile.Flush();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}


