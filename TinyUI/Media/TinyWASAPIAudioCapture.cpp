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
		TinyWASAPIAudioCapture::TinyWASAPIAudioCapture(DWORD dwFlag, DWORD dwLatency)
			:m_dwFlag(dwFlag),
			m_dwLatency(dwLatency)
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}

		TinyWASAPIAudioCapture::~TinyWASAPIAudioCapture()
		{
			Close();
		}

		void TinyWASAPIAudioCapture::OnDataAvailable(BYTE* bits, LONG size, DWORD dwFlag, LPVOID lpParameter)
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
			m_waveEx.Reset(new BYTE[sizeof(WAVEFORMATEX) + ps->cbSize]);
			memcpy(m_waveEx, (BYTE*)ps, sizeof(WAVEFORMATEX) + ps->cbSize);
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				//事件模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwFlag, 0, 0, ps, NULL);
				if (FAILED(hRes))
					goto MMERROR;
			}
			else
			{
				//主动模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwFlag, 100 * MFTIMES_PER_MS, 0, ps, NULL);
				if (FAILED(hRes))
					goto MMERROR;
			}
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
			{
				hRes = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClientLB);
				if (FAILED(hRes))
					goto MMERROR;
				hRes = m_audioClientLB->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, 100 * MFTIMES_PER_MS, 0, ps, NULL);
				if (FAILED(hRes))
					goto MMERROR;
				hRes = m_audioClientLB->SetEventHandle(m_sampleReady);
				if (FAILED(hRes))
					goto MMERROR;
			}
			if (m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				hRes = m_audioClient->SetEventHandle(m_sampleReady);
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

		BOOL TinyWASAPIAudioCapture::Reset()
		{
			HRESULT hRes = m_audioClient->Reset();
			if (FAILED(hRes))
				return FALSE;
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

		BOOL TinyWASAPIAudioCapture::GetStreamLatency(REFERENCE_TIME& latency)
		{
			return m_audioClient->GetStreamLatency(&latency) == S_OK;
		}
		WAVEFORMATEX* TinyWASAPIAudioCapture::GetFormat() const
		{
			if (m_waveEx)
			{
				return reinterpret_cast<WAVEFORMATEX*>(m_waveEx.Ptr());
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
			WAVEFORMATEX* pFormat = GetFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			HANDLE waits[2] = { m_audioStop ,m_sampleReady };
			BOOL recording = FALSE;
			while (!recording)
			{
				DWORD dwMS = m_dwFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : MFTIMES_PER_MS * m_dwLatency / 2;
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, dwMS);
				switch (dwRes)
				{
				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
					OnSampleDone(pFormat->nBlockAlign);
					break;
				case WAIT_FAILED:
				case WAIT_ABANDONED:
					recording = TRUE;
					break;
				}
			}
		}

		void TinyWASAPIAudioCapture::OnSampleDone(UINT32 blockAlign)
		{
			DWORD	dwFlags = 0;
			UINT32	available = 0;
			UINT64	devicePosition = 0;
			UINT64	qpcPosition = 0;
			BYTE*	bits = NULL;
			if (SUCCEEDED(m_audioCapture->GetBuffer(&bits, &available, &dwFlags, &devicePosition, &qpcPosition)))
			{
				OnDataAvailable(bits, available * blockAlign, dwFlags, this);
				m_audioCapture->ReleaseBuffer(available);
			}
		}
	}
}
