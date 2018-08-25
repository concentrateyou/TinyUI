#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include "ByteQueue.h"
#include <dsound.h>

namespace MediaSDK
{
	class WASAPIAudioInputStream : public AudioInputStream
	{
	public:
		WASAPIAudioInputStream();
		virtual ~WASAPIAudioInputStream();
		BOOL Initialize(const AudioParameters& params, const string& deviceID);
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioOutputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		void Close() OVERRIDE;
		BOOL SetVolume(FLOAT volume) OVERRIDE;
		BOOL GetVolume(FLOAT* volume) OVERRIDE;
	private:
		WAVEFORMATEX	m_waveI;
		WAVEFORMATEX	m_waveO;
	};
}

