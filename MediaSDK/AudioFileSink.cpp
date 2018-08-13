#include "stdafx.h"
#include "AudioFileSink.h"

namespace MediaSDK
{
	AudioFileSink::AudioFileSink()
	{
	}

	AudioFileSink::~AudioFileSink()
	{
	}

	BOOL AudioFileSink::Create(const AudioParameters& params, LPTSTR pzFile)
	{
		m_params = params;
		if (!m_wave.Create(pzFile, params.GetFormat()))
			return FALSE;
		return TRUE;
	}

	LONG AudioFileSink::OnOutput(INT64 timestamp, AudioPacket* packet)
	{
		if (packet != NULL)
		{
			return m_wave.Write(reinterpret_cast<BYTE*>(packet->data()), packet->size());
		}
		return -1;
	}

	void AudioFileSink::OnError()
	{

	}
}