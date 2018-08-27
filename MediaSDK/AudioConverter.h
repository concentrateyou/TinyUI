#pragma once
#include "AudioPacket.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	/// <summary>
	///ÒôÆµ×ª»»
	/// </summary>
	class AudioConverter
	{
		DISALLOW_COPY_AND_ASSIGN(AudioConverter)
	public:
		AudioConverter(const AudioParameters& input, const AudioParameters& output);
		~AudioConverter();
	public:

	private:
		void HandleError(HRESULT hRes);
	};
}


