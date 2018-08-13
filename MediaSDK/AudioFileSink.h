#pragma once
#include "MediaSDK.h"
#include "AudioInputStream.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	class AudioFileSink : public AudioInputStream::AudioOutputCallback
	{
		DISALLOW_COPY_AND_ASSIGN(AudioFileSink)
	public:
		AudioFileSink();
		virtual ~AudioFileSink();
	public:
		BOOL Create(const AudioParameters& params, LPTSTR pzFile);
	public:
		LONG OnOutput(INT64 timestamp, AudioPacket* packet) OVERRIDE;
		void OnError() OVERRIDE;
	private:
		TinyWaveFile	m_wave;
		AudioParameters m_params;
	};
}