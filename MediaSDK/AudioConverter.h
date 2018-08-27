#pragma once
#include "AudioPacket.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	/// <summary>
	///��Ƶת��
	/// </summary>
	class AudioConverter
	{
		DISALLOW_COPY_AND_ASSIGN(AudioConverter)
	public:
		AudioConverter(const AudioParameters& src, const AudioParameters& dst);
		~AudioConverter();
	private:
		void HandleError(HRESULT hRes);
	};
}


