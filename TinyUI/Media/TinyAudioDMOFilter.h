#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyAudioDMOFilter : public AudioObserver
		{
		public:
			void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;

		};
	};
}
