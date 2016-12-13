#include "../stdafx.h"
#include "TinyWASAPIAudioCapture.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPIAudioCapture::TinyWASAPIAudioCapture()
			:m_dwStreamFlag(AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST),
			m_bCapturing(FALSE)
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}

		TinyWASAPIAudioCapture::~TinyWASAPIAudioCapture()
		{
			Close();
		}
		void TinyWASAPIAudioCapture::Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback, DWORD dwStreamFlag)
		{
			m_dwStreamFlag = dwStreamFlag;
			m_callback = std::move(callback);
		}
		BOOL TinyWASAPIAudioCapture::Open(const Name& name, WAVEFORMATEX* pFMT)
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
		BOOL TinyWASAPIAudioCapture::OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT)
		{
			ASSERT(mmDevice);
			HRESULT hRes = S_OK;
			BOOL bFlag = (pFMT == NULL);
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = mmDevice->GetState(&state);
			if (hRes != S_OK)
				goto MMERROR;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (hRes != S_OK)
				goto MMERROR;
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
			//AUDCLNT_E_UNSUPPORTED_FORMAT
			hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwStreamFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
			if (hRes != S_OK)
				goto MMERROR;
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			//Ò»°ã¶¼ÊÇ960
			hRes = m_audioClient->GetBufferSize(&m_count);
			if (hRes != S_OK)
				goto MMERROR;
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
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
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
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
			hRes = m_audioClient->GetService(__uuidof(IChannelAudioVolume), (void**)&m_channelVolume);
			if (hRes != S_OK)
				goto MMERROR;
			hRes = m_audioClient->GetService(__uuidof(IAudioSessionControl), (void**)&m_audioSession);
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
		BOOL TinyWASAPIAudioCapture::Start()
		{
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			if (!m_task.Submit(BindCallback(&TinyWASAPIAudioCapture::OnMessagePump, this)))
				return FALSE;
			HRESULT hRes = S_OK;
			if (m_audioClient)
			{
				hRes = m_audioClient->Start();
				if (hRes != S_OK)
					return FALSE;
			}
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Start();
				if (hRes != S_OK)
					return FALSE;
			}
			m_bCapturing = TRUE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Stop()
		{
			HRESULT hRes = S_OK;
			if (m_audioClient)
			{
				hRes = m_audioClient->Stop();
				if (hRes != S_OK)
					return FALSE;
			}
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Stop();
				if (hRes != S_OK)
					return FALSE;
			}
			m_audioStop.SetEvent();
			m_task.Close(INFINITE);
			if (m_audioClient)
			{
				hRes = m_audioClient->Reset();
				if (hRes != S_OK)
					return FALSE;
			}
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Reset();
				if (hRes != S_OK)
					return FALSE;
			}
			m_bCapturing = FALSE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Reset()
		{
			if (m_audioClient)
			{
				HRESULT hRes = m_audioClient->Reset();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::SetVolume(FLOAT volume)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->SetMasterVolume(volume, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetVolume(FLOAT* volume)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->GetMasterVolume(volume) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::SetMute(BOOL bMute)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->SetMute(bMute, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetMute(BOOL* bMute)
		{
			if (!m_audioVolume)
				return FALSE;
			return m_audioVolume->GetMute(bMute) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetStreamLatency(REFERENCE_TIME& latency)
		{
			if (!m_audioClient)
				return FALSE;
			return m_audioClient->GetStreamLatency(&latency) == S_OK;
		}
		BOOL TinyWASAPIAudioCapture::IsCapturing() const
		{
			return m_bCapturing;
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
			if (m_bCapturing && Stop())
			{
				m_audioVolume.Release();
				m_audioCapture.Release();
				m_audioClient.Release();
				m_audioClientLB.Release();
				return FALSE;
			}
			m_audioVolume.Release();
			m_audioCapture.Release();
			m_audioClient.Release();
			m_audioClientLB.Release();
			return TRUE;
		}
		void TinyWASAPIAudioCapture::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetInputFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			HANDLE waits[2] = { m_audioStop ,m_sampleReady };
			BOOL bFlag = TRUE;
			while (bFlag)
			{
				DWORD dwMS = m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : MILLISECONDS_TO_VISUALIZE / 2;
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
				{
					DWORD	dwFlags = 0;
					UINT32	count = 0;
					BYTE*	bits = NULL;
					if (SUCCEEDED(m_audioCapture->GetBuffer(&bits, &count, &dwFlags, NULL, NULL)))
					{
						if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT)
						{
							bits = NULL;
						}
						OnDataReceive(bits, count, this);
						m_audioCapture->ReleaseBuffer(count);
					}
				}
				break;
				}
			}
		}

		void TinyWASAPIAudioCapture::OnDataReceive(BYTE* bits, LONG count, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, count, lpParameter);
			}
		}
	}
}
