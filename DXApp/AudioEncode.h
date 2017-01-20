#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
#include "Media/TinyMFResampler.h"
#include "Media/TinyWave.h"
#include "IO/TinyIO.h"
#include "FFmpegResampler.h"
using namespace TinyUI::IO;
using namespace DShow;


namespace DXApp
{
	class AudioEncode
	{
	public:
		AudioEncode(const AudioCapture::Name& name, const AudioCaptureParam& param);
		virtual ~AudioEncode();
		BOOL				Open(DWORD dwAudioRate);
		BOOL				Close();
		BOOL				Submit();
		FaacEncode&			GetEncode();
		AudioCapture&		GetCapture();
		AudioCaptureParam&	GetParam();
	private:
		void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
	private:
		DWORD					m_dwOffset;
		DWORD					m_dwINC;
		FaacEncode				m_aac;
		AudioCapture			m_capture;
		AudioCapture::Name		m_name;
		AudioCaptureParam		m_audioParam;
	};
}



