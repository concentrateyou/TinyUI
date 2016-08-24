#include "stdafx.h"
#include "AudioCaptureParam.h"

namespace Media
{
	AudioCaptureParam::AudioCaptureParam()
	{

	}

	AudioCaptureParam::~AudioCaptureParam()
	{
	}

	GUID AudioCaptureParam::MediaType()
	{
		return MEDIATYPE_Audio;
	}

	BOOL AudioCaptureParam::IsValid() const
	{
		return TRUE;
	}

	std::string AudioCaptureParam::ToString() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}
