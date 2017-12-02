#include "stdafx.h"
#include "MAudioDSP.h"
#include "MShow.h"

namespace MShow
{
	/*INT BitReverse(INT index, INT size)
	{
		INT j2;
		INT j1 = index;
		INT k = 0;
		for (INT i = 1; i <= size; i++)
		{
			j2 = j1 / 2;
			k = 2 * k + j1 - 2 * j2;
			j1 = j2;
		}
		return k;
	};
	FLOAT* FFTDB(FLOAT* bits, INT size, INT& iterationsO)
	{
		INT iterations = (INT)(log(size) / log(2));
		INT iterations1 = iterations - 1;
		INT iterations2 = size / 2;
		FLOAT* reals = new FLOAT[size];
		FLOAT* imags = new FLOAT[size];
		FLOAT* decibel = new FLOAT[iterations2];
		iterationsO = iterations1;
		FLOAT tr = 0.0F;
		FLOAT ti = 0.0F;
		FLOAT p = 0.0F;
		FLOAT arg = 0.0F;
		FLOAT c = 0.0F;
		FLOAT s = 0.0F;
		for (INT i = 0; i < size; i++)
		{
			reals[i] = bits[i];
			imags[i] = 0.0F;
		}
		INT k = 0;
		for (INT l = 1; l <= iterations; l++)
		{
			while (k < size)
			{
				for (INT i = 1; i <= iterations2; i++)
				{
					p = BitReverse(k >> iterations1, iterations);
					arg = 6.283185F * p / size;
					c = (FLOAT)cos(arg);
					s = (FLOAT)sin(arg);
					tr = reals[k + iterations2] * c + imags[k + iterations2] * s;
					ti = imags[k + iterations2] * c - reals[k + iterations2] * s;
					reals[k + iterations2] = reals[k] - tr;
					imags[k + iterations2] = imags[k] - ti;
					reals[k] += tr;
					imags[k] += ti;
					k++;
				}
				k += iterations2;
			}
			k = 0;
			iterations1--;
			iterations2 = iterations2 / 2;
		}
		k = 0;
		INT r;
		while (k < size)
		{
			r = BitReverse(k, iterations);
			if (r > k)
			{
				tr = reals[k];
				ti = imags[k];
				reals[k] = reals[r];
				imags[k] = imags[r];
				reals[r] = tr;
				imags[r] = ti;
			}
			k++;
		}
		for (INT i = 0; i < size / 2; i++)
		{
			decibel[i] = 10.0 * log10((FLOAT)(sqrt((reals[i] * reals[i]) + (imags[i] * imags[i]))));
		}
		SAFE_DELETE_ARRAY(reals);
		SAFE_DELETE_ARRAY(imags);
		return decibel;
	}*/


	//////////////////////////////////////////////////////////////////////////
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
	/*	if (!m_audioFFT.Initialize(2048))
			return FALSE;*/
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
		m_timer.SetCallback(23, BindCallback(&MAudioDSP::OnTimer, this));
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
		m_timer.SetCallback(23, BindCallback(&MAudioDSP::OnTimer, this));
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
		m_timer.SetCallback(23, BindCallback(&MAudioDSP::OnTimer, this));
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
		m_timer.Close();
		m_resampler.Close();
		if (!m_audioDSP.Close())
		{
			LOG(ERROR) << "AudioDSP Close Fail";
			return FALSE;
		}
		LOG(INFO) << "AudioDSP Close OK";
		return TRUE;
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
		TinyAutoLock lock(m_lock);
		m_buffer.Add(bits, size);
	}

	void MAudioDSP::OnTimer()
	{
		TinyAutoLock lock(m_lock);
		if (m_audioDSP.IsCapturing())
		{
			if (m_buffer.GetSize() >= 4096)
			{
				memcpy(m_bits, m_buffer.GetPointer(), 4096);
				m_buffer.Remove(0, 4096);
				if (!m_callback.IsNull())
				{
					m_callback(m_bits, 4096);
				}
				/*INT k = 0;
				for (INT i = 0; i < 4096;i += 2)
				{
					m_samples[k] = ToINT16(m_bits + i);
					k++;
				}
				FLOAT* wFFT = m_audioFFT.Calculate(m_samples, 2048);
				FLOAT kk = 0;
				for (INT i = 0;i < 1024;i++)
				{
					kk += wFFT[i];
				}
				kk /= 1024.0F;
				if (kk < 0)
				{
					kk = 0;
				}
				TRACE("Value:%f\n", kk);*/
			}
		}
		else
		{
			m_buffer.Clear();
			ZeroMemory(m_bits, 4096);
			if (!m_callback.IsNull())
			{
				m_callback(m_bits, 4096);
			}
		}
	}
}
