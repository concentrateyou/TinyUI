#include "stdafx.h"
#include "AudioConverter.h"

namespace MediaSDK
{
	AudioConverter::AudioConverter(const AudioParameters& src, const AudioParameters& dst)
	{

	}

	AudioConverter::~AudioConverter()
	{

	}

	void AudioConverter::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "AudioConverter error " << hRes;
	}
}
