#include "stdafx.h"
#include "MAudioDSP.h"

namespace MShow
{
	MAudioDSP::MAudioDSP()
	{
	}


	MAudioDSP::~MAudioDSP()
	{
	}

	BOOL MAudioDSP::Initialize(Callback<void(BYTE*, LONG)>&& callback)
	{
		m_callback = std::move(callback);
		m_audioDSP.Initialize(BindCallback(&MAudioDSP::OnDSP, this));
		WAVEFORMATEX waveFMTI;
		ZeroMemory(&waveFMTI, sizeof(waveFMTI));
		waveFMTI.cbSize = 0;
		waveFMTI.wFormatTag = WAVE_FORMAT_PCM;
		waveFMTI.nChannels = 1;
		waveFMTI.nSamplesPerSec = 16000;
		waveFMTI.wBitsPerSample = 16;
		waveFMTI.nBlockAlign = 2;
		waveFMTI.nAvgBytesPerSec = 32000;
		WAVEFORMATEX waveFMTO;
		ZeroMemory(&waveFMTO, sizeof(waveFMTO));
		waveFMTO.cbSize = 0;
		waveFMTO.wFormatTag = WAVE_FORMAT_PCM;
		waveFMTO.nChannels = 2;
		waveFMTO.nSamplesPerSec = 44100;
		waveFMTO.wBitsPerSample = 16;
		waveFMTO.nBlockAlign = 4;
		waveFMTO.nAvgBytesPerSec = 176400;
		if (!m_resampler.Open(&waveFMTI, &waveFMTO, BindCallback(&MAudioDSP::OnAudio, this)))
		{
			LOG(ERROR) << "Resampler Open Fail" << endl;
			return FALSE;
		}
		LOG(INFO) << "MAudioDSP Initialize OK" << endl;
		return TRUE;
	}
	BOOL MAudioDSP::Open()
	{
		m_audioDSP.EnableNS(TRUE);
		m_audioDSP.EnableAGC(FALSE);
		vector<TinyWASAPIAudio::Name> captures;
		TinyWASAPIAudio::GetDevices(eCapture, captures);
		vector<TinyWASAPIAudio::Name> renders;
		TinyWASAPIAudio::GetDevices(eRender, renders);
		TinyWASAPIAudio::Name capture;
		for (INT i = 0;i < captures.size();i++)
		{
			BOOL IsMA = FALSE;
			TinyWASAPIAudio::IsMicrophoneArray(captures[i], IsMA);
			if (IsMA)
			{
				capture = captures[i];
				break;
			}
			TinyWASAPIAudio::IsMicrophone(captures[i], IsMA);
			if (IsMA)
			{
				capture = captures[i];
				break;
			}
		}
		if (capture.IsEmpty())
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Capture Device" << endl;
			return FALSE;
		}
		if (renders.size() == 0)
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Render Device" << endl;
			return FALSE;
		}
		WAVEFORMATEX waveFMTI;
		ZeroMemory(&waveFMTI, sizeof(waveFMTI));
		waveFMTI.cbSize = 0;
		waveFMTI.wFormatTag = WAVE_FORMAT_PCM;
		waveFMTI.nChannels = 1;
		waveFMTI.nSamplesPerSec = 16000;
		waveFMTI.wBitsPerSample = 16;
		waveFMTI.nBlockAlign = 2;
		waveFMTI.nAvgBytesPerSec = 32000;
		WAVEFORMATEX waveFMTO;
		ZeroMemory(&waveFMTO, sizeof(waveFMTO));
		waveFMTO.cbSize = 0;
		waveFMTO.wFormatTag = WAVE_FORMAT_PCM;
		waveFMTO.nChannels = 2;
		waveFMTO.nSamplesPerSec = 44100;
		waveFMTO.wBitsPerSample = 16;
		waveFMTO.nBlockAlign = 4;
		waveFMTO.nAvgBytesPerSec = 176400;
		if (!m_audioDSP.Open(renders[0], capture, &waveFMTI))
		{
			LOG(ERROR) << "[MAudioDSP] Open Fail" << endl;
			return FALSE;
		}
		m_waveFile.Create("D:\\mshow.wav", &waveFMTO);
		LOG(INFO) << "[MAudioDSP] Open OK" << endl;
		return TRUE;
	}
	BOOL MAudioDSP::Start()
	{
		if (!m_audioDSP.Start())
		{
			LOG(ERROR) << "AudioDSP Start Fail" << endl;
			return FALSE;
		}
		m_timer.SetCallback(23, BindCallback(&MAudioDSP::OnTimer, this));
		LOG(INFO) << "AudioDSP Start OK" << endl;
		return TRUE;
	}
	BOOL MAudioDSP::Stop()
	{
		m_timer.Close();
		if (!m_audioDSP.Stop())
		{
			LOG(ERROR) << "AudioDSP Stop Fail" << endl;
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Stop OK" << endl;
		return TRUE;
	}
	BOOL MAudioDSP::Close()
	{
		m_resampler.Close();
		if (!m_audioDSP.Close())
		{
			LOG(ERROR) << "AudioDSP Close Fail" << endl;
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Close OK" << endl;
		return TRUE;
	}
	void MAudioDSP::OnDSP(BYTE* bits, LONG size, LPVOID lpParameter)
	{
		m_resampler.Resample(bits, size);
	}
	void MAudioDSP::OnAudio(BYTE* bits, LONG size, LPVOID lpParameter)
	{
		TinyAutoLock lock(m_lock);
		m_buffer.Add(bits, size);
	}
	void MAudioDSP::OnTimer()
	{
		TinyAutoLock lock(m_lock);
		if (m_buffer.GetSize() >= 4096)
		{
			memcpy(m_data, m_buffer.GetPointer(), 4096);
			m_buffer.Remove(0, 4096);
			m_waveFile.Write(m_data, 4096);
			if (!m_callback.IsNull())
			{
				m_callback(m_data, 4096);
			}
		}
	}
}
