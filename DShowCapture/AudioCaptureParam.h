#pragma once
#include "DShowCommon.h"

namespace DShow
{
	class AudioCaptureFormat
	{
	public:
		AudioCaptureFormat();
		~AudioCaptureFormat();
		WAVEFORMATEX GetFormat() const;
		void SetFormat(const WAVEFORMATEX& wfx);
		string ToString() const;
		static WAVEFORMATEX GetDefaultFormat();
	private:
		WAVEFORMATEX	m_wft;
	};

	class AudioCaptureParam
	{
	public:
		AudioCaptureParam();
		~AudioCaptureParam();
	public:
		AudioCaptureFormat RequestFormat;
	};
}
