#include "../stdafx.h"
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		AudioParameter::AudioParameter(const WAVEFORMATEX& wfx)
			:m_wfx(wfx)
		{

		}

		WAVEFORMATEX AudioParameter::GetFormat() const
		{
			return m_wfx;
		}
	};
}
