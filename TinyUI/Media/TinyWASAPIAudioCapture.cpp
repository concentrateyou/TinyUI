#include "../stdafx.h"
#include "TinyWASAPIAudioCapture.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPIAudioCapture::TinyWASAPIAudioCapture(DWORD dwFlag)
			:m_dwFlag(dwFlag),
			m_blockAlign(0),
			m_bufferFrames(0)
		{
			m_samplesReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}

		TinyWASAPIAudioCapture::~TinyWASAPIAudioCapture()
		{
			Close();
		}

		void TinyWASAPIAudioCapture::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{

		}

		BOOL TinyWASAPIAudioCapture::Open()
		{
			WAVEFORMATEX* ps = NULL;
			TinyComPtr<IMMDeviceEnumerator> enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			hRes = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_mmDevice);
			if (FAILED(hRes))
				return FALSE;
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = m_mmDevice->GetState(&state);
			if (FAILED(hRes))
				return FALSE;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_audioClient->GetMixFormat(&ps);
			if (FAILED(hRes))
				goto MMERROR;
			m_blockAlign = ps->nBlockAlign;
			DWORD dwSize = sizeof(WAVEFORMATEX) + ps->cbSize;
			m_waveEx.Reset(new BYTE[dwSize]);
			memcpy(m_waveEx, (const char*)ps, dwSize);
			hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwFlag, 0, 0, ps, NULL);
			if (FAILED(hRes))
				goto MMERROR;
			hRes = m_audioClient->GetBufferSize(&m_bufferFrames);
			if (FAILED(hRes))
				goto MMERROR;
			hRes = m_audioClient->GetDevicePeriod(&m_defaultDevicePeriod, &m_minimumDevicePeriod);
			if (FAILED(hRes))
				goto MMERROR;
			hRes = m_audioClient->GetStreamLatency(&m_latency);
			if (FAILED(hRes))
				goto MMERROR;
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
			{
				hRes = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClientLB);
				if (FAILED(hRes))
					goto MMERROR;
				hRes = m_audioClientLB->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, 0, 0, ps, NULL);
				if (FAILED(hRes))
					goto MMERROR;
				hRes = m_audioClientLB->SetEventHandle(m_samplesReady);
				if (FAILED(hRes))
					goto MMERROR;
			}
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				hRes = m_audioClient->SetEventHandle(m_samplesReady);
				if (FAILED(hRes))
					goto MMERROR;
			}
			hRes = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_audioCapture);
			if (FAILED(hRes))
				goto MMERROR;
			hRes = m_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_audioVolume);
			if (FAILED(hRes))
				goto MMERROR;
			return TRUE;
		MMERROR:
			if (ps)
			{
				CoTaskMemFree(ps);
				ps = NULL;
			}
			return FALSE;
		}

		BOOL TinyWASAPIAudioCapture::Start()
		{
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			Closure s = BindCallback(&TinyWASAPIAudioCapture::OnMessagePump, this);
			if (!m_task.Submit(s))
				return FALSE;
			HRESULT hRes = m_audioClient->Start();
			if (FAILED(hRes))
				return FALSE;
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Start();
				if (FAILED(hRes))
					return FALSE;
			}
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Stop()
		{
			m_audioStop.SetEvent();
			HRESULT hRes = m_audioClient->Stop();
			if (FAILED(hRes))
				return FALSE;
			m_task.Close(INFINITE);
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::SetVolume(FLOAT volume)
		{
			return m_audioVolume->SetMasterVolume(volume, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetVolume(FLOAT* volume)
		{
			return m_audioVolume->GetMasterVolume(volume) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::SetMute(BOOL bMute)
		{
			return m_audioVolume->SetMute(bMute, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetMute(BOOL* bMute)
		{
			return m_audioVolume->GetMute(bMute) == S_OK;
		}

		REFERENCE_TIME	TinyWASAPIAudioCapture::GetLatency() const
		{
			return m_latency;
		}

		BOOL TinyWASAPIAudioCapture::Close()
		{
			if (Stop() && m_audioClientLB)
			{
				return SUCCEEDED(m_audioClientLB->Stop());
			}
			return TRUE;
		}

		void TinyWASAPIAudioCapture::OnMessagePump()
		{
			DWORD dwTaskIndex = 0;
			HANDLE hMM = AvSetMmThreadCharacteristics("Audio", &dwTaskIndex);
			ASSERT(hMM);
			AvSetMmThreadPriority(hMM, AVRT_PRIORITY_CRITICAL);
			BOOL error = FALSE;
			BOOL recording = TRUE;
			HANDLE waits[2] = { m_audioStop ,m_samplesReady };
			while (recording && !error)
			{
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
				switch (dwRes)
				{
				case WAIT_OBJECT_0 + 0:
					recording = FALSE;
					break;
				case WAIT_OBJECT_0 + 1:
					FillSample();
					break;
				default:
					error = TRUE;
					break;
				}
			}
			AvRevertMmThreadCharacteristics(hMM);
		}

		void TinyWASAPIAudioCapture::FillSample()
		{
			BYTE* bits = NULL;
			UINT32 size = 0;
			DWORD dwFlags = 0;
			UINT64 devicePosition = 0;
			UINT64 qpcPosition = 0;
			if (SUCCEEDED(m_audioCapture->GetBuffer(&bits, &size, &dwFlags, &devicePosition, &qpcPosition)))
			{
				OnDataAvailable(bits, size * m_blockAlign, NULL);
				m_audioCapture->ReleaseBuffer(size);
			}
		}

	}
}
