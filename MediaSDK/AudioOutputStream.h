#pragma once
#include "AudioPacket.h"

namespace MediaSDK
{
	class AudioOutputStream
	{
		DISALLOW_COPY_AND_ASSIGN(AudioOutputStream)
	public:
		class  AudioInputCallback
		{
		public:
			virtual ~AudioInputCallback();
			virtual LONG	OnInput(INT64 delay, INT64 timestamp, AudioPacket* packet) = 0;
			virtual void	OnError() = 0;
		};
	public:
		AudioOutputStream();
		virtual ~AudioOutputStream();
		virtual BOOL Open() = 0;
		virtual BOOL Start(AudioInputCallback* callback) = 0;
		virtual BOOL Stop() = 0;
		virtual BOOL GetVolume(FLOAT* volume) = 0;
		virtual BOOL SetVolume(FLOAT volume) = 0;
		virtual void Close() = 0;
	};
}