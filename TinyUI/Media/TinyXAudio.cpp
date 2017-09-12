#include "../stdafx.h"
#include "TinyXAudio.h"

namespace TinyUI
{
	namespace Media
	{
		VoiceCallback::VoiceCallback(Closure&& callback)
			:m_callback(std::move(callback))
		{

		}
		VoiceCallback::~VoiceCallback()
		{

		}
		void VoiceCallback::OnVoiceProcessingPassStart(UINT32 BytesRequired)
		{
		}
		void VoiceCallback::OnVoiceProcessingPassEnd()
		{
		}
		void VoiceCallback::OnStreamEnd()
		{
			m_callback();
		}
		void VoiceCallback::OnBufferStart(void* ps)
		{
		}
		void VoiceCallback::OnBufferEnd(void* ps)
		{
		}
		void VoiceCallback::OnLoopEnd(void* ps)
		{
		}
		void VoiceCallback::OnVoiceError(void*, HRESULT)
		{
		}
		BOOL VoiceCallback::Lock(DWORD dwMS)
		{
			return m_event.Lock(dwMS);
		}
		void VoiceCallback::Unlock()
		{
			m_event.SetEvent();
		}
		//////////////////////////////////////////////////////////////////////////
		TinyXAudio::TinyXAudio()
			:m_pMasteringVoice(NULL),
			m_pSourceVoice(NULL),
			m_voiceCallback(BindCallback(&TinyXAudio::OnVoice, this))
		{

		}
		TinyXAudio::~TinyXAudio()
		{

		}
		BOOL TinyXAudio::Open(const WAVEFORMATEX* pFMT, Closure&& callback)
		{
			HRESULT hRes = XAudio2Create(&m_audio, 0, XAUDIO2_DEFAULT_PROCESSOR);
			if (hRes != S_OK)
				return FALSE;
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			debug.BreakMask = XAUDIO2_LOG_ERRORS;
			m_audio->SetDebugConfiguration(&debug, 0);
#endif
			hRes = m_audio->CreateMasteringVoice(&m_pMasteringVoice);
			if (hRes != S_OK)
				goto _ERROR;
			hRes = m_audio->CreateSourceVoice(&m_pSourceVoice, pFMT, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voiceCallback);
			if (hRes != S_OK)
				goto _ERROR;
			m_callback = std::move(callback);
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
		BOOL TinyXAudio::Start()
		{
			if (!m_pSourceVoice)
				return FALSE;
			m_pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
			return TRUE;
		}
		BOOL TinyXAudio::Stop()
		{
			if (!m_pSourceVoice)
				return FALSE;
			m_pSourceVoice->Stop();
			return TRUE;
		}
		BOOL TinyXAudio::Close()
		{
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
		void TinyXAudio::OnVoice()
		{
			m_callback();
		}
	};
}
