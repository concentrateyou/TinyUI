#pragma once
#include "MShowCommon.h"
#include "Media/TinyWave.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Media/TinyMFResampler.h"
#include "MAudioQueue.h"
#include "MAudioDSP.h"
#include "Common/TinyTime.h"
#include "Common/TinyCore.h"
using namespace TinyUI;
using namespace TinyUI::Media;

namespace MShow
{
#define AAC_SIZE 4096

	class MAudioDSP
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioDSP)
	public:
		MAudioDSP();
		~MAudioDSP();
		BOOL Initialize();
		BOOL Open();
		BOOL Open(const TinyWASAPIAudio::Name& capture, const TinyWASAPIAudio::Name& speaker);
		BOOL Open(const GUID& capture, const GUID& speaker);
		BOOL Start();
		BOOL Stop();
		BOOL Close();
		MAudioQueue& GetAudioQueue();
		BOOL IsCapturing() const;
		BOOL IsEmpty();
	private:
		void OnDSP(BYTE* bits, LONG size, LPVOID lpParameter);
		void OnAudio(BYTE* bits, LONG size, LPVOID lpParameter);
	private:
		BYTE						m_bits[4096];
		TinyAudioDSPCapture			m_audioDSP;
		TinyMFResampler				m_resampler;
		TinyBufferArray<BYTE>		m_buffer;
		MAudioQueue					m_audioQueue;
		TinyWaveFile				m_waveFile;
		WAVEFORMATEX				m_waveFMTI;
		WAVEFORMATEX				m_waveFMTO;
	};
}

