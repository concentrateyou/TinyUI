#pragma once
#include "MShowCommon.h"
#include "AudioCaptureParam.h"
#include "AudioCapture.h"
using namespace DShow;

namespace MShow
{
	/// <summary>
	/// “Ù∆µ≤∂ªÒ»ŒŒÒ
	/// </summary>
	class MAudioCapture
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioCapture)
	public:
		MAudioCapture();
		~MAudioCapture();
		WAVEFORMATEX GetFormat();
		BOOL Initialize();
		BOOL Open(Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback);
		void Close();
		BOOL Start();
		BOOL Stop();
	private:
		AudioCapture		m_capture;
		AudioCapture::Name	m_name;
		AudioCaptureParam	m_param;
	};
}


