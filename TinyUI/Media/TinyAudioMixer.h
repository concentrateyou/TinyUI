#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// “Ù∆µªÏ“Ù∆˜
		/// </summary>
		class TinyAudioMixer : public AudioObserver
		{
		public:
			void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;

		};
	};
}
