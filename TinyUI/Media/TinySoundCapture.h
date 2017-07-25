#pragma once
#include "TinyWave.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <vector>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// DirectSound“Ù∆µ≤∂ªÒ
		/// </summary>
		class TinySoundCapture
		{
		public:
			TinySoundCapture();
			virtual ~TinySoundCapture();
		public:
			BOOL Initialize();
		private:
			static BOOL CALLBACK DSEnumCallback(LPGUID, LPCSTR, LPCSTR, LPVOID);
		private:
			std::vector<GUID>	m_devices;
		};
	}
}

