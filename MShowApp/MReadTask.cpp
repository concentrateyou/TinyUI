#include "stdafx.h"
#include "MReadTask.h"

namespace MShow
{
	MReadTask::MReadTask()
		:m_close(FALSE)
	{
	}


	MReadTask::~MReadTask()
	{
	}

	BOOL MReadTask::Initialize(LPCSTR pzURL)
	{
		m_reader.Close();
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();

	}

	BOOL MReadTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MReadTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;

		}
	}

}
