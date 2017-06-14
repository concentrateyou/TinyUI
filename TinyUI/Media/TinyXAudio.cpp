#include "../stdafx.h"
#include "TinyXAudio.h"

namespace TinyUI
{
	namespace Media
	{
		VoiceCallback::VoiceCallback()
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
		//////////////////////////////////////////////////////////////////////////
		TinyXAudio::TinyXAudio()
			:m_pMasteringVoice(NULL),
			m_pSourceVoice(NULL)
		{

		}
		TinyXAudio::~TinyXAudio()
		{

		}
		BOOL TinyXAudio::Initialize(const WAVEFORMATEX* pFMT)
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
				return FALSE;
			hRes = m_audio->CreateSourceVoice(&m_pSourceVoice, pFMT);
			if (hRes != S_OK)
				return FALSE;
			return SUCCEEDED(hRes);
		}
	};
}
