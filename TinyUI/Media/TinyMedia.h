#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include <mmsystem.h>
#include <dshow.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")

namespace TinyUI
{
	namespace Media
	{
		class AudioParameter
		{
		public:
			AudioParameter(const WAVEFORMATEX& wfx);
			WAVEFORMATEX GetFormat() const;
		private:
			WAVEFORMATEX m_wfx;
		};
	};
}
