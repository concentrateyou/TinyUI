#include "stdafx.h"
#include "AudioFileSource.h"

namespace MediaSDK
{
	AudioFileSource::AudioFileSource()
	{
	}

	AudioFileSource::~AudioFileSource()
	{
	}

	BOOL AudioFileSource::Open(LPTSTR pzFile)
	{
		if (!PathFileExists(pzFile))
			return FALSE;
		return m_waveFile.Open(pzFile);
	}

	WAVEFORMATEX* AudioFileSource::GetFormat() const
	{
		return m_waveFile.GetFormat();
	}

	LONG AudioFileSource::OnInput(INT64 delay, INT64 timestamp, UINT32 skips, AudioPacket* packet)
	{
		LONG numberOfBytesRead = 0;
		m_waveFile.Read(reinterpret_cast<BYTE*>(packet->data()), packet->size(), &numberOfBytesRead);
		INT count = numberOfBytesRead / ((packet->BitsPerSample() * packet->Channels()) / 8);
		return min(count, packet->count());
	}

	void AudioFileSource::OnError()
	{

	}
}