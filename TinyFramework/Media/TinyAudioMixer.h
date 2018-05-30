#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyFramework
{
	namespace Media
	{
		/// <summary>
		/// “Ù∆µªÏ“Ù∆˜
		/// </summary>
		class TinyAudioMixer : public AudioObserver
		{
		public:
			void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		};
	};
}
