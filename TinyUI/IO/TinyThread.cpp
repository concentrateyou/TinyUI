#include "../stdafx.h"
#include "TinyThread.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TaskItem::TaskItem()
			:m_dwDelay(0)
		{

		}
		TaskItem::~TaskItem()
		{

		}
		DWORD TaskItem::GetDelay() const
		{
			return m_dwDelay;
		}
		void TaskItem::SetDelay(DWORD dwDelay)
		{
			m_dwDelay = dwDelay;
		}
		string TaskItem::GetName() const
		{
			return m_name;
		}
		void TaskItem::SetName(const string& name)
		{
			m_name = name;
		}
		//////////////////////////////////////////////////////////////////////////
		TaskRunner::TaskRunner()
		{

		}
		TaskRunner::~TaskRunner()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		TinyTLS::TinyTLS()
			:m_dwTlsIndex(0)
		{
			m_dwTlsIndex = TlsAlloc();
			if (m_dwTlsIndex == TLS_OUT_OF_INDEXES)
				throw system_error(GetLastError(), system_category());
		}
		void TinyTLS::SetValue(LPVOID ps)
		{
			TlsSetValue(m_dwTlsIndex, ps);
		}
		LPVOID TinyTLS::GetValue()
		{
			return TlsGetValue(m_dwTlsIndex);
		}
		TinyTLS::~TinyTLS()
		{
			TlsFree(m_dwTlsIndex);
		}
	}
}
