#pragma once
#include "TinyCommon.h"
#include "TinyCore.h"
#include <dbghelp.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <io.h>
#pragma comment(lib, "dbghelp.lib")

namespace TinyUI
{
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
	string GetDefaultLogFile();
	/// <summary>
	/// 日志类
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
}



