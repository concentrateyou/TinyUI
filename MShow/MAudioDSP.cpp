#include "stdafx.h"
#include "MAudioDSP.h"
#include "MShowController.h"
#include "MShow.h"

namespace MShow
{
	MAudioDSP::MAudioDSP()
	{
	}


	MAudioDSP::~MAudioDSP()
	{
	}

	BOOL MAudioDSP::Initialize()
	{
		m_audioDSP.Initialize(BindCallback(&MAudioDSP::OnDSP, this));
		ZeroMemory(&m_waveFMTI, sizeof(m_waveFMTI));
		m_waveFMTI.cbSize = 0;
		m_waveFMTI.wFormatTag = WAVE_FORMAT_PCM;
		m_waveFMTI.nChannels = 1;
		m_waveFMTI.nSamplesPerSec = 16000;
		m_waveFMTI.wBitsPerSample = 16;
		m_waveFMTI.nBlockAlign = 2;
		m_waveFMTI.nAvgBytesPerSec = 32000;
		ZeroMemory(&m_waveFMTO, sizeof(m_waveFMTO));
		m_waveFMTO.cbSize = 0;
		m_waveFMTO.wFormatTag = WAVE_FORMAT_PCM;
		m_waveFMTO.nChannels = 2;
		m_waveFMTO.nSamplesPerSec = 44100;
		m_waveFMTO.wBitsPerSample = 16;
		m_waveFMTO.nBlockAlign = 4;
		m_waveFMTO.nAvgBytesPerSec = 176400;
		LOG(INFO) << "MAudioDSP Initialize OK" << endl;
		return TRUE;
	}
	BOOL MAudioDSP::Open(const TinyWASAPIAudio::Name& capture, const TinyWASAPIAudio::Name& speaker)
	{
		m_audioDSP.EnableNS(TRUE);
		m_audioDSP.EnableAGC(FALSE);
		if (capture.IsEmpty())
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Capture Device";
			return FALSE;
		}
		if (!m_resampler.Open(&m_waveFMTI, &m_waveFMTO, BindCallback(&MAudioDSP::OnAudio, this)))
		{
			LOG(ERROR) << "Resampler Open Fail";
			return FALSE;
		}
		if (!m_audioDSP.Open(speaker, capture, &m_waveFMTI))
		{
			LOG(ERROR) << "[MAudioDSP] Open Fail";
			return FALSE;
		}
		MAppConfig& config = MShow::MShowApp::GetInstance().AppConfig();
		string szFile = config.GetSaveFile();
		if (szFile.size() > 0)
		{
			if (!m_waveFile.Create((LPTSTR)szFile.c_str(), &m_waveFMTO))
			{
				LOG(ERROR) << "[MAudioDSP] Create Wave File: " << szFile << " Fail";
			}
		}
		LOG(INFO) << "[MAudioDSP] Open OK";
		return TRUE;
	}
	BOOL MAudioDSP::Open(const GUID& capture, const GUID& speaker)
	{
		m_audioDSP.EnableNS(TRUE);
		m_audioDSP.EnableAGC(FALSE);
		if (IsEqualGUID(capture, GUID_NULL))
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Capture Device";
			return FALSE;
		}
		if (IsEqualGUID(speaker, GUID_NULL))
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Speaker Device";
			return FALSE;
		}
		if (!m_resampler.Open(&m_waveFMTI, &m_waveFMTO, BindCallback(&MAudioDSP::OnAudio, this)))
		{
			LOG(ERROR) << "Resampler Open Fail";
			return FALSE;
		}
		if (!m_audioDSP.Open(speaker, capture, &m_waveFMTI))
		{
			LOG(ERROR) << "[MAudioDSP] Open Fail";
			return FALSE;
		}
		MAppConfig& config = MShow::MShowApp::GetInstance().AppConfig();
		string szFile = config.GetSaveFile();
		if (szFile.size() > 0)
		{
			if (!m_waveFile.Create((LPTSTR)szFile.c_str(), &m_waveFMTO))
			{
				LOG(ERROR) << "[MAudioDSP] Create Wave File: " << szFile << " Fail";
			}
		}
		LOG(INFO) << "[MAudioDSP] Open OK";
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
		for (size_t i = 0;i < captures.size();i++)
		{
			BOOL IsMA = FALSE;
			TinyWASAPIAudio::IsMicrophone(captures[i], IsMA);
			if (IsMA)
			{
				capture = captures[i];
				break;
			}
		}
		if (capture.IsEmpty())
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Capture Device";
			return FALSE;
		}
		if (renders.size() == 0)
		{
			LOG(ERROR) << "[MAudioDSP] Can't Get Render Device";
			return FALSE;
		}
		if (!m_resampler.Open(&m_waveFMTI, &m_waveFMTO, BindCallback(&MAudioDSP::OnAudio, this)))
		{
			LOG(ERROR) << "Resampler Open Fail";
			return FALSE;
		}
		if (!m_audioDSP.Open(renders[0], capture, &m_waveFMTI))
		{
			LOG(ERROR) << "[MAudioDSP] Open Fail";
			return FALSE;
		}
		MAppConfig& config = MShow::MShowApp::GetInstance().AppConfig();
		string szFile = config.GetSaveFile();
		if (szFile.size() > 0)
		{
			if (!m_waveFile.Create((LPTSTR)szFile.c_str(), &m_waveFMTO))
			{
				LOG(ERROR) << "[MAudioDSP] Create Wave File: " << szFile << " Fail";
			}
		}
		LOG(INFO) << "[MAudioDSP] Open OK";
		return TRUE;
	}
	BOOL MAudioDSP::Start()
	{
		if (!m_audioDSP.Start())
		{
			LOG(ERROR) << "AudioDSP Start Fail";
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Start OK";
		return TRUE;
	}
	BOOL MAudioDSP::Stop()
	{
		if (!m_audioDSP.Stop())
		{
			LOG(ERROR) << "AudioDSP Stop Fail";
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Stop OK";
		return TRUE;
	}
	BOOL MAudioDSP::Close()
	{
		m_resampler.Close();
		if (!m_audioDSP.Close())
		{
			LOG(ERROR) << "AudioDSP Close Fail";
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Close OK";
		return TRUE;
	}
	MAudioQueue& MAudioDSP::GetAudioQueue()
	{
		return m_audioQueue;
	}
	BOOL MAudioDSP::IsCapturing() const
	{
		return m_audioDSP.IsCapturing();
	}
	BOOL MAudioDSP::IsEmpty()
	{
		return m_audioDSP.IsEmpty();
	}
	void MAudioDSP::OnDSP(BYTE* bits, LONG size, LPVOID lpParameter)
	{
		m_resampler.Resample(bits, size);
	}

	void MAudioDSP::OnAudio(BYTE* bits, LONG size, LPVOID lpParameter)
	{
		m_buffer.Add(bits, size);
		if (m_buffer.GetSize() >= AAC_SIZE)
		{
			if (m_audioQueue.GetAllocSize() == 0)
			{
				INT count = 5;
				m_audioQueue.Initialize(count, AAC_SIZE + 4);
			}
			AUDIO_SAMPLE sample = { 0 };
			sample.size = AAC_SIZE;
			sample.bits = static_cast<BYTE*>(m_audioQueue.Alloc());
			memcpy_s(sample.bits + 4, AAC_SIZE, m_buffer.GetPointer(), AAC_SIZE);
			m_buffer.Remove(0, AAC_SIZE);
			m_audioQueue.Push(sample);
		}
	}
}
