#pragma once
#include "AudioBus.h"

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
			virtual INT		OnInput(INT64 delay, INT64 timestamp) = 0;
			virtual void	OnError() = 0;
		};
	public:
		AudioOutputStream();
		virtual ~AudioOutputStream();
		virtual BOOL Open() = 0;
		virtual BOOL Start(AudioInputCallback* callback) = 0;
		virtual BOOL Stop() = 0;
		virtual BOOL GetVolume(DOUBLE* volume) = 0;
		virtual BOOL SetVolume(DOUBLE volume) = 0;
		virtual void Close() = 0;
	};
}