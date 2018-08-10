#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class AudioInputStream
	{
		DISALLOW_COPY_AND_ASSIGN(AudioInputStream)
	public:
		class  AudioOutputCallback
		{
		public:
			virtual ~AudioOutputCallback();
			virtual INT		Onoutput(INT64 delay, INT64 timestamp, UINT32 skips, AudioPacket* packet) = 0;
			virtual void	OnError() = 0;
		};
	public:
		AudioInputStream();
		virtual~AudioInputStream();
	public:
		virtual BOOL Open() = 0;
		virtual BOOL Start(AudioOutputCallback* callback) = 0;
		virtual BOOL Stop() = 0;
		virtual void Close() = 0;
		virtual BOOL SetVolume(DOUBLE volume) = 0;
		virtual BOOL GetVolume(DOUBLE* volume) = 0;
	};
}



