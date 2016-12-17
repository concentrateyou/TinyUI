#include "../stdafx.h"
#include "TinyWASAPIAudioRender.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPIAudioRender::TinyWASAPIAudioRender()
			:m_dwStreamFlag(DEFAULT_RENDER_AUDCLNT_STREAMFLAGS),
			m_shareMode(AUDCLNT_SHAREMODE_EXCLUSIVE),
			m_hnsPeriod(0),
			m_bPlaying(FALSE)
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}
		TinyWASAPIAudioRender::~TinyWASAPIAudioRender()
		{
			Close();
		}
		void TinyWASAPIAudioRender::Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback, DWORD dwStreamFlag, AUDCLNT_SHAREMODE shareMode)
		{
			m_dwStreamFlag = dwStreamFlag;
			m_shareMode = shareMode;
			m_callback = std::move(callback);
		}
		BOOL TinyWASAPIAudioRender::Open(const Name& name, WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IMMDeviceEnumerator> enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (hRes != S_OK)
				return FALSE;
			return OpenClient(mmDevice, pFMT);
		}
		BOOL TinyWASAPIAudioRender::OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT)
		{
			ASSERT(mmDevice);
			HRESULT hRes = S_OK;
			BOOL bFlag = (pFMT == NULL);
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = mmDevice->GetState(&state);
			if (hRes != S_OK)
				return FALSE;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (hRes != S_OK)
				return FALSE;
			if (pFMT == NULL)
			{
				hRes = m_audioClient->GetMixFormat(&pFMT);
				if (hRes != S_OK)
					goto MMERROR;
			}
			REFERENCE_TIME defaultPeriod = 0;
			REFERENCE_TIME minimumDevicePeriod = 0;
			hRes = m_audioClient->GetDevicePeriod(&defaultPeriod, &minimumDevicePeriod);
			if (hRes != S_OK)
				goto MMERROR;
			m_hnsPeriod = defaultPeriod;
			switch (m_shareMode)
			{
			case AUDCLNT_SHAREMODE_SHARED:
			{
				m_hnsPeriod = MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS;
				hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, m_hnsPeriod, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			break;
			case AUDCLNT_SHAREMODE_EXCLUSIVE:
			{
				if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
				{
					hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, m_hnsPeriod, m_hnsPeriod, pFMT, NULL);
					if (hRes != S_OK)
					{
						if (hRes != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
						{
							goto MMERROR;
						}
						hRes = m_audioClient->GetBufferSize(&m_count);
						if (hRes != S_OK)
							goto MMERROR;
						m_hnsPeriod = ((REFERENCE_TIME)(MFTIMES_PER_SEC * m_count / pFMT->nSamplesPerSec + 0.5));
						hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, m_hnsPeriod, m_hnsPeriod, pFMT, NULL);
						if (hRes != S_OK)
							goto MMERROR;
					}

				}
				else
				{
					hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, m_hnsPeriod, 0, pFMT, NULL);
					if (hRes != S_OK)
						goto MMERROR;
				}
			}
			break;
			}
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			::memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			hRes = m_audioClient->GetBufferSize(&m_count);
			if (hRes != S_OK)
				goto MMERROR;
			REFERENCE_TIME latency = 0;
			hRes = m_audioClient->GetStreamLatency(&latency);
			if (hRes != S_OK)
				goto MMERROR;
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				hRes = m_audioClient->SetEventHandle(m_sampleReady);
				if (hRes != S_OK)
					goto MMERROR;
			}
			hRes = m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_audioRender);
			if (hRes != S_OK)
				goto MMERROR;
			hRes = m_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_audioVolume);
			if (hRes != S_OK)
				goto MMERROR;
			return TRUE;
		MMERROR:
			if (pFMT && bFlag)
			{
				CoTaskMemFree(pFMT);
				pFMT = NULL;
			}
			return FALSE;
		}
		BOOL TinyWASAPIAudioRender::Start()
		{
			if (!m_audioClient)
				return FALSE;
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			if (!m_task.Submit(BindCallback(&TinyWASAPIAudioRender::OnMessagePump, this)))
				return FALSE;
			HRESULT hRes = S_OK;
			switch (m_shareMode)
			{
			case AUDCLNT_SHAREMODE_SHARED:
			{
				UINT32 padding = 0;
				hRes = m_audioClient->GetCurrentPadding(&padding);
				if (hRes != S_OK)
					return FALSE;
				UINT32 size = m_count - padding;
				if (size > 0)
				{
					BYTE* data = NULL;
					hRes = m_audioRender->GetBuffer(size, &data);
					if (hRes != S_OK)
						return FALSE;
					hRes = m_audioRender->ReleaseBuffer(size, AUDCLNT_BUFFERFLAGS_SILENT);
					if (hRes != S_OK)
						return FALSE;
				}
			}
			break;
			}
			hRes = m_audioClient->Start();
			if (hRes != S_OK)
				return FALSE;
			m_bPlaying = TRUE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioRender::Stop()
		{
			if (!m_audioClient)
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_audioClient->Stop();
			if (hRes != S_OK)
				return FALSE;
			m_audioStop.SetEvent();
			m_task.Close(INFINITE);
			hRes = m_audioClient->Reset();
			if (hRes != S_OK)
				return FALSE;
			m_bPlaying = FALSE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioRender::Reset()
		{
			if (!m_audioClient)
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_audioClient->Reset();
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioRender::SetVolume(FLOAT volume)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->SetMasterVolume(volume, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioRender::GetVolume(FLOAT* volume)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->GetMasterVolume(volume) == S_OK;
		}

		BOOL TinyWASAPIAudioRender::SetMute(BOOL bMute)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->SetMute(bMute, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioRender::GetMute(BOOL* bMute)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->GetMute(bMute) == S_OK;
		}

		BOOL TinyWASAPIAudioRender::GetStreamLatency(REFERENCE_TIME& latency)
		{
			if (!m_audioClient)
				return FALSE;
			return m_audioClient->GetStreamLatency(&latency) == S_OK;
		}

		BOOL TinyWASAPIAudioRender::IsPlaying() const
		{
			return m_bPlaying;
		}

		WAVEFORMATEX* TinyWASAPIAudioRender::GetInputFormat() const
		{
			if (m_waveFMT)
			{
				return reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			}
			return NULL;
		}
		BOOL TinyWASAPIAudioRender::Close()
		{
			if (m_bPlaying && !Stop())
			{
				m_audioVolume.Release();
				m_audioRender.Release();
				m_audioClient.Release();
				return FALSE;
			}
			m_audioVolume.Release();
			m_audioRender.Release();
			m_audioClient.Release();
			return TRUE;
		}

		BOOL TinyWASAPIAudioRender::Render(WAVEFORMATEX* pFMT)
		{
			ASSERT(m_audioClient);
			BYTE* data = NULL;
			UINT32 padding = 0;
			UINT32 available = 0;
			HRESULT hRes = S_OK;
			switch (m_shareMode)
			{
			case AUDCLNT_SHAREMODE_EXCLUSIVE:
			{
				if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
				{
					UINT32 available = m_count;
					hRes = m_audioRender->GetBuffer(available, &data);
					if (hRes != S_OK)
						return FALSE;
					OnDataAvailable(data, available * pFMT->nBlockAlign, this);
					hRes = m_audioRender->ReleaseBuffer(m_count, 0);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = m_audioClient->GetCurrentPadding(&padding);
					if (hRes != S_OK)
						return FALSE;
					available = m_count - padding;
					if (available > 0)
					{
						hRes = m_audioRender->GetBuffer(available, &data);
						if (hRes != S_OK)
							return FALSE;
						OnDataAvailable(data, available * pFMT->nBlockAlign, this);
						hRes = m_audioRender->ReleaseBuffer(available, 0);
						if (hRes != S_OK)
							return FALSE;
					}
				}
			}
			break;
			case AUDCLNT_SHAREMODE_SHARED:
			{
				hRes = m_audioClient->GetCurrentPadding(&padding);
				if (hRes != S_OK)
					return FALSE;
				available = m_count - padding;
				if (available > 0)
				{
					hRes = m_audioRender->GetBuffer(available, &data);
					if (hRes != S_OK)
						return FALSE;
					OnDataAvailable(data, available * pFMT->nBlockAlign, this);
					hRes = m_audioRender->ReleaseBuffer(available, 0);
					if (hRes != S_OK)
						return FALSE;
				}
			}
			break;
			}
			return TRUE;
		}

		void TinyWASAPIAudioRender::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetInputFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			HRESULT hRes = S_OK;
			HANDLE waits[2] = { m_audioStop ,m_sampleReady };
			UINT32 currentCaptureIndex = 0;
			BOOL bFlag = TRUE;
			while (bFlag)
			{
				DWORD dwMS = m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : static_cast<DWORD>(m_hnsPeriod / MFTIMES_PER_MS / 2);
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, dwMS);
				switch (dwRes)
				{
				case WAIT_OBJECT_0 + 0:
				case WAIT_FAILED:
				case WAIT_ABANDONED:
					bFlag = FALSE;
					break;
				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
					bFlag = Render(pFMT);
					break;
				}
			}
		}

		void TinyWASAPIAudioRender::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
	}
}
