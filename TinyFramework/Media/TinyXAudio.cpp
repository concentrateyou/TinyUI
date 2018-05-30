#include "../stdafx.h"
#include "TinyXAudio.h"
#include "../Common/TinyTime.h"
#include "../Common/TinyLogging.h"

namespace TinyFramework
{
	namespace Media
	{
		VoiceCallback::VoiceCallback()
			:m_handle(NULL)
		{
			m_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
		}
		VoiceCallback::~VoiceCallback()
		{
			if (m_handle != NULL)
			{
				CloseHandle(m_handle);
				m_handle = NULL;
			}
		}
		void VoiceCallback::SetCallback(Closure&& callback)
		{
			m_callback = std::move(callback);
		}
		void VoiceCallback::OnVoiceProcessingPassStart(UINT32 BytesRequired)
		{
		}
		void VoiceCallback::OnVoiceProcessingPassEnd()
		{
		}
		void VoiceCallback::OnStreamEnd()
		{
			if (!m_callback.IsNull())
			{
				m_callback();
			}
		}
		void VoiceCallback::OnBufferStart(void* ps)
		{
		}
		void VoiceCallback::OnBufferEnd(void* ps)
		{
			if (m_handle != NULL)
			{
				::SetEvent(m_handle);
			}
		}
		void VoiceCallback::OnLoopEnd(void* ps)
		{
		}
		void VoiceCallback::OnVoiceError(void*, HRESULT hRes)
		{

		}
		BOOL VoiceCallback::Lock(DWORD dwMS)
		{
			if (!m_handle)
				return FALSE;
			DWORD dwRes = ::WaitForSingleObject(m_handle, dwMS);
			if (dwRes == WAIT_OBJECT_0 || dwRes == WAIT_ABANDONED)
				return TRUE;
			else
				return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyXAudio::TinyXAudio()
			:m_pMasteringVoice(NULL),
			m_pSourceVoice(NULL),
			m_dwIndex(0)
		{
		}
		TinyXAudio::~TinyXAudio()
		{

		}
		void TinyXAudio::SetCallback(Closure&& callback)
		{
			m_voiceCallback.SetCallback(std::move(callback));
		}
		BOOL TinyXAudio::Open(const WAVEFORMATEX* pFMT)
		{
			Close();
			HRESULT hRes = XAudio2Create(&m_audio, 0, XAUDIO2_DEFAULT_PROCESSOR);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[XAudio] [Open] XAudio2Create:" << hRes;
				return FALSE;
			}
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
			debug.LogFileline = TRUE;
			debug.LogFunctionName = TRUE;
			debug.LogThreadID = TRUE;
			debug.LogTiming = TRUE;
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			m_audio->SetDebugConfiguration(&debug, 0);
#endif
			hRes = m_audio->CreateMasteringVoice(&m_pMasteringVoice,
				(pFMT) ? pFMT->nChannels : XAUDIO2_DEFAULT_CHANNELS,
				(pFMT) ? pFMT->nSamplesPerSec : XAUDIO2_DEFAULT_SAMPLERATE);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[XAudio] [Open] CreateMasteringVoice:" << hRes;
				goto _ERROR;
			}
			hRes = m_audio->CreateSourceVoice(&m_pSourceVoice, pFMT, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voiceCallback);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[XAudio] [Open] CreateSourceVoice:" << hRes;
				goto _ERROR;
			}
			m_dwIndex = 0;
			for (INT i = 0;i < MAX_BUFFER_COUNT;i++)
			{
				m_array[i].Reset(new BYTE[MAX_STREAM_BUFFER_SIZE]);
				if (!m_array[i])
					goto _ERROR;
			}
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			return TRUE;
		_ERROR:
			LOG(ERROR) << "[XAudio] [Open] " << hRes;
			Close();
			return FALSE;
		}
		BOOL TinyXAudio::Play(BYTE* bits, LONG size)
		{
			if (m_pSourceVoice != NULL)
			{
				XAUDIO2_BUFFER buffer = { 0 };
				buffer.pContext = &m_voiceCallback;
				buffer.pAudioData = bits;
				buffer.AudioBytes = size;
				buffer.Flags = XAUDIO2_END_OF_STREAM;
				HRESULT hRes = m_pSourceVoice->SubmitSourceBuffer(&buffer, NULL);
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[XAudio] [Play] SubmitSourceBuffer:" << hRes;
					return FALSE;
				}
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::Play(BYTE* bits, LONG size, DWORD dwMS)
		{
			ASSERT(size <= MAX_STREAM_BUFFER_SIZE);
			if (m_pSourceVoice != NULL)
			{
				XAUDIO2_VOICE_STATE state;
				for (;; )
				{
					m_pSourceVoice->GetState(&state);
					if (state.BuffersQueued < (MAX_BUFFER_COUNT - 1))
						break;
					BOOL bRes = m_voiceCallback.Lock(dwMS);
					if (!bRes)
					{
						LOG(ERROR) << "[TinyXAudio] [Play] Timeout";
					}
				}
				memcpy_s(m_array[m_dwIndex], size, bits, size);
				XAUDIO2_BUFFER buffer = { 0 };
				buffer.pContext = &m_voiceCallback;
				buffer.pAudioData = m_array[m_dwIndex];
				buffer.AudioBytes = size;
				HRESULT hRes = m_pSourceVoice->SubmitSourceBuffer(&buffer);
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [Play] SubmitSourceBuffer:" << hRes;
					m_dwIndex = 0;
					return FALSE;
				}
				m_dwIndex++;
				m_dwIndex %= MAX_BUFFER_COUNT;
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::SetVolume(DWORD dwVolume)
		{
			if (m_pSourceVoice != NULL)
			{
				FLOAT val = dwVolume / 100.0F;
				HRESULT hRes = m_pSourceVoice->SetVolume(val);
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [SetVolume] SetVolume:" << hRes;
					return FALSE;
				}
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::SetChannelVolumes(UINT channels, DWORD dwVolume)
		{
			if (m_pSourceVoice != NULL)
			{
				FLOAT val = dwVolume / 100.0F;
				HRESULT hRes = m_pSourceVoice->SetChannelVolumes(channels, &val);
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [SetChannelVolumes] SetChannelVolumes:" << hRes;
					return FALSE;
				}
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::Suspend()
		{
			if (m_audio != NULL)
			{
				m_audio->StopEngine();
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::Resume()
		{
			if (m_audio != NULL)
			{
				m_audio->StartEngine();
				return TRUE;
			}
			return FALSE;

		}
		BOOL TinyXAudio::Start()
		{
			Stop();
			if (m_pSourceVoice != NULL)
			{
				HRESULT hRes = m_pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [Start] Start:" << hRes;
					return FALSE;
				}
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::Stop()
		{
			m_dwIndex = 0;
			if (m_pSourceVoice != NULL)
			{
				HRESULT hRes = m_pSourceVoice->Stop();
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [Stop] Stop:" << hRes;
					return FALSE;
				}
				hRes = m_pSourceVoice->FlushSourceBuffers();
				if (FAILED(hRes))
				{
					LOG(ERROR) << "[TinyXAudio] [Stop] FlushSourceBuffers:" << hRes;
				}
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyXAudio::Close()
		{
			BOOL bRes = TRUE;
			m_dwIndex = 0;
			if (m_pMasteringVoice != NULL)
			{
				m_pMasteringVoice->DestroyVoice();
				m_pMasteringVoice = NULL;
			}
			if (m_pSourceVoice != NULL)
			{
				m_pSourceVoice->DestroyVoice();
				m_pSourceVoice = NULL;
			}
			m_audio.Release();
			return bRes;
		}
		BOOL TinyXAudio::Reset()
		{
			Close();
			return Open(reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr()));
		}
	};
}
