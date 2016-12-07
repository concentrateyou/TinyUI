#include "../stdafx.h"
#include "TinyThread.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TaskItem::TaskItem(DWORD dwTaskType)
			:m_dwTaskType(dwTaskType)
		{

		}
		TaskItem::~TaskItem()
		{

		}
		DWORD TaskItem::GetTaskType() const
		{
			return m_dwTaskType;
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
			if ((m_dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
			{
				throw system_error(GetLastError(), system_category());
			}
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
