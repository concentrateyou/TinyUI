#include "../stdafx.h"
#include "TinyWASAPIAudioCapture.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyScopedAvrt::TinyScopedAvrt(LPCSTR pzTaskName)
			:m_hMM(NULL),
			m_dwTaskIndex(0)
		{
			m_hMM = AvSetMmThreadCharacteristics(pzTaskName, &m_dwTaskIndex);
		}
		TinyScopedAvrt::~TinyScopedAvrt()
		{
			if (m_hMM != NULL)
			{
				AvRevertMmThreadCharacteristics(m_hMM);
				m_hMM = NULL;
			}
		}
		TinyScopedAvrt::operator HANDLE() const
		{
			return m_hMM;
		}
		BOOL TinyScopedAvrt::SetPriority(AVRT_PRIORITY priority)
		{
			ASSERT(m_hMM);
			return AvSetMmThreadPriority(m_hMM, priority);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWASAPIAudioCapture::TinyWASAPIAudioCapture(DWORD dwFlag, BOOL bUse16)
			:m_dwFlag(dwFlag),
			m_bUse16(bUse16),
			m_bufferFrames(0),
			m_captureBufferSize(0)
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}

		TinyWASAPIAudioCapture::~TinyWASAPIAudioCapture()
		{
			Close();
		}
		void TinyWASAPIAudioCapture::Initialize(Callback<void(BYTE*, LONG, LPVOID)>& callback)
		{
			m_callback = callback;
		}
		BOOL TinyWASAPIAudioCapture::Open()
		{
			WAVEFORMATEX* pFMT = NULL;
			TinyComPtr<IMMDeviceEnumerator> enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			hRes = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &mmDevice);
			if (hRes != S_OK)
				return FALSE;
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = mmDevice->GetState(&state);
			if (hRes != S_OK)
				return FALSE;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_audioClient->GetMixFormat(&pFMT);
			if (hRes != S_OK)
				goto MMERROR;
			if (m_bUse16)
			{
				if ((pFMT->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
					((pFMT->wFormatTag == WAVE_FORMAT_EXTENSIBLE) &&
					(reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pFMT)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)))
				{
					pFMT->wFormatTag = WAVE_FORMAT_PCM;
					pFMT->wBitsPerSample = 16;
					pFMT->nBlockAlign = (pFMT->nChannels  * pFMT->wBitsPerSample) / 8;
					pFMT->nAvgBytesPerSec = pFMT->nSamplesPerSec * pFMT->nBlockAlign;
					pFMT->cbSize = 0;
				}
			}
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				//事件模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			else
			{
				//主动模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			//一般都是960
			hRes = m_audioClient->GetBufferSize(&m_bufferFrames);
			if (FAILED(hRes))
				return hRes;
			REFERENCE_TIME defaultPeriod = 0;
			hRes = m_audioClient->GetDevicePeriod(&defaultPeriod, NULL);
			if (FAILED(hRes))
				return hRes;
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
			{
				hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClientLB);
				if (hRes != S_OK)
					goto MMERROR;
				hRes = m_audioClientLB->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
				hRes = m_audioClientLB->SetEventHandle(m_sampleReady);
				if (hRes != S_OK)
					goto MMERROR;
			}
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				hRes = m_audioClient->SetEventHandle(m_sampleReady);
				if (hRes != S_OK)
					goto MMERROR;
			}
			hRes = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_audioCapture);
			if (hRes != S_OK)
				goto MMERROR;
			hRes = m_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_audioVolume);
			if (hRes != S_OK)
				goto MMERROR;
			m_captureBufferSize = m_bufferFrames * ((pFMT->nChannels*pFMT->wBitsPerSample) / 8);
			m_captureBuffer.Reset(new BYTE[m_captureBufferSize]);
			m_captureBuffer[m_captureBufferSize - 1] = -32768;
			return TRUE;
		MMERROR:
			if (pFMT)
			{
				CoTaskMemFree(pFMT);
				pFMT = NULL;
			}
			return FALSE;
		}

		BOOL TinyWASAPIAudioCapture::Start()
		{
			ASSERT(m_audioClient);
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			Closure s = BindCallback(&TinyWASAPIAudioCapture::OnMessagePump, this);
			if (!m_task.Submit(s))
				return FALSE;
			HRESULT hRes = m_audioClient->Start();
			if (hRes != S_OK)
				return FALSE;
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Start();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Stop()
		{
			m_audioStop.SetEvent();
			m_task.Close(INFINITE);
			if (m_audioClient)
			{
				HRESULT hRes = m_audioClient->Stop();
				if (hRes != S_OK)
					return FALSE;
			}
			m_audioVolume.Release();
			m_audioCapture.Release();
			m_audioClient.Release();
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Reset()
		{
			HRESULT hRes = m_audioClient->Reset();
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::SetVolume(FLOAT volume)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->SetMasterVolume(volume, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetVolume(FLOAT* volume)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->GetMasterVolume(volume) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::SetMute(BOOL bMute)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->SetMute(bMute, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetMute(BOOL* bMute)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->GetMute(bMute) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetStreamLatency(REFERENCE_TIME& latency)
		{
			ASSERT(m_audioClient);
			return m_audioClient->GetStreamLatency(&latency) == S_OK;
		}
		WAVEFORMATEX* TinyWASAPIAudioCapture::GetInputFormat() const
		{
			if (m_waveFMT)
			{
				return reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			}
			return NULL;
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
			WAVEFORMATEX* pFMT = GetInputFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			HANDLE waits[2] = { m_audioStop ,m_sampleReady };
			BOOL	_break = FALSE;
			UINT32	currentCaptureIndex = 0;
			while (!_break)
			{
				DWORD dwMS = m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : 10;
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, dwMS);
				switch (dwRes)
				{
				case WAIT_OBJECT_0 + 0:
				case WAIT_FAILED:
				case WAIT_ABANDONED:
					_break = TRUE;
					break;
				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
				{
					DWORD	dwFlags = 0;
					UINT32	count = 0;
					BYTE*	bits = NULL;
					if (SUCCEEDED(m_audioCapture->GetBuffer(&bits, &count, &dwFlags, NULL, NULL)))
					{
						OnDataAvailable(bits, count, this);
						m_audioCapture->ReleaseBuffer(count);
					}
				}
				break;
				}
			}
		}

		void TinyWASAPIAudioCapture::OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, count, lpParameter);
			}
		}
	}
}
