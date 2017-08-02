#include "stdafx.h"
#include "MP3DecodeTask.h"

namespace Decode
{
	MP3DecodeTask::MP3DecodeTask(TinyUI::Callback<void(BYTE*, LONG)>&& callback)
		:m_bBreak(FALSE),
		m_callback(std::move(callback))
	{
		ZeroMemory(&m_waveFMT, sizeof(m_waveFMT));
	}


	MP3DecodeTask::~MP3DecodeTask()
	{
	}
	BOOL MP3DecodeTask::Submit(LPCSTR pzFile)
	{
		if (!m_mp3File.Open(pzFile))
			goto _ERROR;
		m_decode.Initialize(BindCallback(&MP3DecodeTask::OnMP3Audio, this));
		if (!m_decode.Open())
			goto _ERROR;
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MP3DecodeTask::OnMessagePump, this));
	_ERROR:
		m_mp3File.Close();
		m_decode.Close();
		return FALSE;
	}
	BOOL MP3DecodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_mp3File.Close();
			m_decode.Close();
			return TRUE;
		}
		return FALSE;
	}
	WAVEFORMATEX MP3DecodeTask::GetFormat() const
	{
		return m_waveFMT;
	}
	void MP3DecodeTask::OnMessagePump()
	{
		BYTE bits[1024 * 4];
		LONGLONG timestamp = 0;
		for (;;)
		{
			if (m_bBreak)
				break;
			LONG numberOfBytesRead = 0;
			if (!m_mp3File.Read(bits, 1024 * 4, &numberOfBytesRead, timestamp))
			{
				TRACE("MP3 Read Fail\n");
				break;
			}
			if (numberOfBytesRead <= 0)
			{
				TRACE("MP3 Read End\n");
				break;
			}
			else
			{
				if (!m_decode.Decode(bits, numberOfBytesRead))
				{
					TRACE("MP3 Decode Fail\n");
					break;
				}
			}
		}
	}
	void MP3DecodeTask::OnMP3Audio(BYTE* bits, LONG size, LPVOID ps)
	{
		if (size == 0)
		{
			m_waveFMT = *reinterpret_cast<WAVEFORMATEX*>(ps);
		}
		else
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size);
			}
		}
	}
}