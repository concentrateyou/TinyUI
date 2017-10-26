#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include <dbghelp.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <io.h>
#include <yvals.h>
#pragma comment(lib, "dbghelp.lib")

namespace TinyUI
{
	string GetDefaultDumpFile();
	/// <summary>
	/// Symbol上下文
	/// </summary>
	class SymbolContext
	{
		DISALLOW_COPY_AND_ASSIGN(SymbolContext)
	public:
		static SymbolContext* GetInstance();
		DWORD GetError() const;
		void OutputTraceToStream(const void* const* trace, size_t count, std::ostream* os);
	private:
		SymbolContext();
		DWORD m_error;
		TinyLock m_lock;
		static SymbolContext* m_pInstance;
	};
	SELECTANY SymbolContext* SymbolContext::m_pInstance = NULL;
	/// <summary>
	/// 栈打印
	/// </summary>
	class StackTrace
	{
		DISALLOW_COPY_AND_ASSIGN(StackTrace)
	public:
		StackTrace();
		StackTrace(const void* const* trace, size_t count);
		StackTrace(const _EXCEPTION_POINTERS* exception_pointers);
		~StackTrace();
		const void* const* Addresses(size_t* count) const;
		void Print() const;
		void OutputToStream(std::ostream* os) const;
		std::string ToString() const;
	private:
		//http://msdn.microsoft.com/en-us/library/bb204633.aspx,
		static const int MaxTraces = 62;
		void* m_trace[MaxTraces];
		size_t m_count;
	};
	/// <summary>
	/// 异常日志类
	/// </summary>
	class LogException
	{
		DISALLOW_COPY_AND_ASSIGN(LogException)
	public:
		LogException(LPCSTR pFileName, INT line);
		~LogException();
		void WriteLog(PEXCEPTION_POINTERS ps);
	private:
		BOOL Initialize(LPCSTR pFileName, INT line);
	private:
		std::ostringstream	m_stream;
		LPCSTR				m_pFileName;
		size_t				m_messageOffset;
		HANDLE				m_hFile;
		const INT			m_line;
	};
	typedef INT LogSeverity;
	const LogSeverity LOG_VERBOSE = -1;
	const LogSeverity LOG_INFO = 0;
	const LogSeverity LOG_WARNING = 1;
	const LogSeverity LOG_ERROR = 2;
	const LogSeverity LOG_FATAL = 3;
	const LogSeverity LOG_NUM_SEVERITIES = 4;
	/// <summary>
	/// 日志类
	/// </summary>
	class LogMessage
	{
		DISALLOW_COPY_AND_ASSIGN(LogMessage)
	public:
		LogMessage(LogSeverity severity);
		LogMessage(LPCSTR pzFile, INT line, LogSeverity severity);
		virtual ~LogMessage();
	public:
		ostream& stream();
	private:
		void Initialize();
		void Initialize(LPCSTR pzFile, INT line);
	private:
		LogSeverity			m_severity;
		ostringstream		m_stream;
		INT					m_line;
	};

	BOOL SetLogFile(LPCSTR pzFile);
	BOOL CloseLogFile();

#define LOG(severity) \
	LogMessage(LOG_##severity).stream()

#define LOG_LINE(severity) \
	LogMessage(__FILE__, __LINE__, LOG_##severity).stream()
}



