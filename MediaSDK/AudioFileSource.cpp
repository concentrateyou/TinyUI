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

	LONG AudioFileSource::OnInput(INT64 delay, INT64 timestamp, UINT32 skips, AudioPacket* packet)
	{
		return -1;
	}

	void AudioFileSource::OnError()
	{

	}
}