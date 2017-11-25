#include "stdafx.h"
#include "MP3Task.h"

namespace Decode
{
	/*MP3Task::MP3Task()
		:m_bBreak(FALSE)
	{
		ZeroMemory(&m_waveFMT, sizeof(m_waveFMT));
	}


	MP3Task::~MP3Task()
	{
	}
	BOOL MP3Task::Submit(LPCSTR pzFile)
	{
		if (!m_reader.Open(pzFile))
			goto _ERROR;
		m_decode.Initialize(BindCallback(&MP3Task::OnMP3Audio, this));
		if (!m_decode.Open())
			goto _ERROR;
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MP3Task::OnMessagePump, this));
	_ERROR:
		m_reader.Close();
		m_decode.Close();
		return FALSE;
	}
	BOOL MP3Task::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_reader.Close();
			m_decode.Close();
			return TRUE;
		}
		return FALSE;
	}
	WAVEFORMATEX MP3Task::GetFormat() const
	{
		return m_waveFMT;
	}
	void MP3Task::OnMessagePump()
	{
		BYTE bits[1024 * 8];
		LONGLONG timestamp = 0;
		for (;;)
		{
			if (m_bBreak)
				break;
			LONG numberOfBytesRead = 0;
			if (!m_reader.Read(bits, 1024 * 8, &numberOfBytesRead, timestamp))
			{
				break;
			}
			if (numberOfBytesRead <= 0)
			{
				break;
			}
			if (!m_decode.Decode(bits, numberOfBytesRead))
			{
				break;
			}
		}
	}
	void MP3Task::OnMP3Audio(BYTE* bits, LONG size, LPVOID ps)
	{
		if (size == 0)
		{
			m_waveFMT = *reinterpret_cast<WAVEFORMATEX*>(ps);
		}
		else
		{
			if (m_residial.GetSize() >= 4096)
			{
				EVENT_AUDIO(m_residial.GetPointer(), 4096);
				m_residial.Remove(0, 4096);
			}
			m_residial.Add(bits, size);
		}
	}*/
}