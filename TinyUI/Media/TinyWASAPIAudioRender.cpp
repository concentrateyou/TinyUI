#include "../stdafx.h"
#include "TinyWASAPIAudioRender.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPIAudioRender::TinyWASAPIAudioRender()
			:m_dwStreamFlag(DEFAULT_RENDER_AUDCLNT_STREAMFLAGS),
			m_shareMode(AUDCLNT_SHAREMODE_SHARED)
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}
		TinyWASAPIAudioRender::~TinyWASAPIAudioRender()
		{
			Close();
		}
		void TinyWASAPIAudioRender::Initialize(DWORD dwStreamFlag, AUDCLNT_SHAREMODE shareMode)
		{
			m_dwStreamFlag = dwStreamFlag;
			m_shareMode = shareMode;
			//m_callback = std::move(callback);
		}
		BOOL TinyWASAPIAudioRender::Open(const Name& name, WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IMMDeviceEnumerator> enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (FAILED(hRes))
				return FALSE;
			return OpenClient(mmDevice, pFMT);
		}
		BOOL TinyWASAPIAudioRender::OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT)
		{
			ASSERT(mmDevice);
			BOOL	bFlag = (pFMT == NULL);
			HRESULT hRes = S_OK;
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = mmDevice->GetState(&state);
			if (FAILED(hRes))
				return FALSE;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (FAILED(hRes))
				return FALSE;
			if (pFMT == NULL)
			{
				hRes = m_audioClient->GetMixFormat(&pFMT);
				if (hRes != S_OK)
					goto MMERROR;
			}
			switch (m_shareMode)
			{
			case AUDCLNT_SHAREMODE_SHARED:
			{
				hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			break;
			case AUDCLNT_SHAREMODE_EXCLUSIVE:
			{
				REFERENCE_TIME s = MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS / 2;
				if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
				{
					hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, s, s, pFMT, NULL);
					if (hRes != S_OK)
						goto MMERROR;
				}
				else
				{
					hRes = m_audioClient->Initialize(m_shareMode, m_dwStreamFlag, s, s, pFMT, NULL);
					if (hRes != S_OK)
						goto MMERROR;
				}
			}
			break;
			}
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			//Ò»°ã¶¼ÊÇ960
			hRes = m_audioClient->GetBufferSize(&m_count);
			if (FAILED(hRes))
				return hRes;
			REFERENCE_TIME latency = 0;
			hRes = m_audioClient->GetStreamLatency(&latency);
			if (FAILED(hRes))
				return hRes;
			REFERENCE_TIME defaultPeriod = 0;
			REFERENCE_TIME minimumDevicePeriod = 0;
			hRes = m_audioClient->GetDevicePeriod(&defaultPeriod, &minimumDevicePeriod);
			if (FAILED(hRes))
				return hRes;
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
			HRESULT hRes = m_audioClient->Start();
			if (FAILED(hRes))
				return FALSE;
			if (m_shareMode == AUDCLNT_SHAREMODE_SHARED)
			{
				UINT32 padding = 0;
				hRes = m_audioClient->GetCurrentPadding(&padding);
				if (hRes != S_OK)
					return FALSE;
				BYTE* bits = NULL;
				UINT32 size = m_count - padding;
				hRes = m_audioRender->GetBuffer(size, &bits);
				if (hRes != S_OK)
					return FALSE;
				hRes = m_audioRender->ReleaseBuffer(size, AUDCLNT_BUFFERFLAGS_SILENT);
				if (hRes != S_OK)
					return FALSE;
			}
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
			return TRUE;
		}

		BOOL TinyWASAPIAudioRender::Reset()
		{
			if (!m_audioClient)
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_audioClient->Reset();
			if (FAILED(hRes))
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
			if (Stop())
			{
				m_audioVolume.Release();
				m_audioRender.Release();
				m_audioClient.Release();
				return TRUE;
			}
			return FALSE;
		}

		BOOL TinyWASAPIAudioRender::Render()
		{
			HRESULT hRes = S_OK;
			BYTE*	bits = NULL;
			LONG	size = 0;
			UINT32	available = 0;
			if (m_shareMode == AUDCLNT_SHAREMODE_SHARED)
			{
				UINT32	padding = 0;
				hRes = m_audioClient->GetCurrentPadding(&padding);
				if (FAILED(hRes))
					return FALSE;
				available = m_count - padding;
			}
			else
			{
				available = m_count;
			}
			return TRUE;
		}

		void TinyWASAPIAudioRender::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetInputFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			BOOL	bBreak = FALSE;
			HRESULT hRes = S_OK;
			HANDLE	waits[2] = { m_audioStop ,m_sampleReady };
			UINT32	currentCaptureIndex = 0;
			while (!bBreak)
			{
				DWORD dwMS = m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : 10;
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, dwMS);
				switch (dwRes)
				{
				case WAIT_OBJECT_0 + 0:
				case WAIT_FAILED:
				case WAIT_ABANDONED:
					bBreak = TRUE;
					break;
				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
					bBreak = Render();
					break;
				}
			}
		}
	}
}
