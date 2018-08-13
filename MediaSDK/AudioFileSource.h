#pragma once
#include "MediaSDK.h"
#include "AudioOutputStream.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	class AudioFileSource : public AudioOutputStream::AudioInputCallback
	{
		DISALLOW_COPY_AND_ASSIGN(AudioFileSource)
	public:
		AudioFileSource();
		virtual ~AudioFileSource();
	public:
		LONG OnInput(INT64 delay, INT64 timestamp, UINT32 skips, AudioPacket* packet) OVERRIDE;
		void OnError() OVERRIDE;
	public:
		void Open(const AudioParameters& params, LPTSTR pzFile);
	private:
		AudioParameters	m_params;
	};
}