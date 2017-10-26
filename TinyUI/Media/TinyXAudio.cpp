#include "../stdafx.h"
#include "TinyXAudio.h"
#include "../Common/TinyTime.h"
#include "../Common/TinyLogging.h"

namespace TinyUI
{
	namespace Media
	{
		VoiceCallback::VoiceCallback()
			:m_handle(NULL)
		{
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
			m_handle = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
#else
			m_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
		}
		VoiceCallback::~VoiceCallback()
		{
			CloseHandle(m_handle);
			m_handle = NULL;
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
			SetEvent(m_handle);
		}
		void VoiceCallback::OnLoopEnd(void* ps)
		{
		}
		void VoiceCallback::OnVoiceError(void*, HRESULT hRes)
		{
			LOG(ERROR) << "OnVoiceError:" << hRes;
		}
		BOOL VoiceCallback::Lock(DWORD dwMS)
		{
			DWORD dwRet = ::WaitForSingleObjectEx(m_handle, dwMS, TRUE);
			if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
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
			HRESULT hRes = XAudio2Create(&m_audio, 0, XAUDIO2_DEFAULT_PROCESSOR);
			if (hRes != S_OK)
				return FALSE;
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
			debug.LogFileline = TRUE;
			debug.LogFunctionName = TRUE;
			debug.LogThreadID = TRUE;
			debug.LogTiming = TRUE;
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			m_audio->SetDebugConfiguration(&debug, 0);
#endif
			hRes = m_audio->CreateMasteringVoice(&m_pMasteringVoice);
			if (hRes != S_OK)
				goto _ERROR;
			hRes = m_audio->CreateSourceVoice(&m_pSourceVoice, pFMT, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voiceCallback);
			if (hRes != S_OK)
				goto _ERROR;
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
			Close();
			return FALSE;
		}
		BOOL TinyXAudio::Fill(BYTE* bits, LONG size)
		{
			if (!m_pSourceVoice)
				return FALSE;
			XAUDIO2_BUFFER buffer = { 0 };
			buffer.pContext = &m_voiceCallback;
			buffer.pAudioData = bits;
			buffer.AudioBytes = size;
			buffer.Flags = XAUDIO2_END_OF_STREAM;
			if (FAILED(m_pSourceVoice->SubmitSourceBuffer(&buffer, NULL)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyXAudio::Fill(BYTE* bits, LONG size, DWORD dwMS)
		{
			ASSERT(size <= MAX_STREAM_BUFFER_SIZE);
			if (!m_pSourceVoice)
				return FALSE;
			XAUDIO2_VOICE_STATE state;
			for (;; )
			{
				m_pSourceVoice->GetState(&state);
				if (state.BuffersQueued < (MAX_BUFFER_COUNT - 1))
					break;
				m_voiceCallback.Lock(dwMS);
			}
			memcpy_s(m_array[m_dwIndex], size, bits, size);
			XAUDIO2_BUFFER buffer = { 0 };
			buffer.pContext = &m_voiceCallback;
			buffer.pAudioData = m_array[m_dwIndex];
			buffer.AudioBytes = size;
			HRESULT hRes = m_pSourceVoice->SubmitSourceBuffer(&buffer);
			if (FAILED(hRes))
			{
				LOG(ERROR) << "[TinyXAudio] SubmitSourceBuffer:" << hRes;
				return FALSE;
			}
			m_dwIndex++;
			m_dwIndex %= MAX_BUFFER_COUNT;
			return TRUE;
		}
		BOOL TinyXAudio::SetVolume(DWORD dwVolume)
		{
			if (!m_pSourceVoice)
				return FALSE;
			FLOAT val = dwVolume / 100.0F;
			return SUCCEEDED(m_pSourceVoice->SetVolume(val));
		}
		BOOL TinyXAudio::SetChannelVolumes(UINT channels, DWORD dwVolume)
		{
			if (!m_pSourceVoice)
				return FALSE;
			FLOAT val = dwVolume / 100.0F;
			return SUCCEEDED(m_pSourceVoice->SetChannelVolumes(channels, &val));
		}
		BOOL TinyXAudio::Start()
		{
			if (!m_pSourceVoice)
				return FALSE;
			m_pSourceVoice->FlushSourceBuffers();
			m_pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
			return TRUE;
		}
		BOOL TinyXAudio::Stop()
		{
			m_dwIndex = 0;
			if (!m_pSourceVoice)
				return FALSE;
			m_pSourceVoice->FlushSourceBuffers();
			m_pSourceVoice->Stop();
			return TRUE;
		}
		BOOL TinyXAudio::Close()
		{
			m_dwIndex = 0;
			if (m_pMasteringVoice != NULL)
			{
				m_pMasteringVoice->DestroyVoice();
				m_pMasteringVoice = NULL;
			}
			if (m_pSourceVoice != NULL)
			{
				m_pSourceVoice->Stop();
				m_pSourceVoice->DestroyVoice();
				m_pSourceVoice = NULL;
			}
			m_audio.Release();
			return TRUE;
		}
		BOOL TinyXAudio::Reset()
		{
			Close();
			return Open(reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr()));
		}
	};
}
