#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// ��Ƶ������
		/// </summary>
		class TinyAudioMixer : public AudioObserver
		{
		public:
			void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;

		};
	};
}
