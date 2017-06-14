#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		//class VoiceCallback : public IXAudio2VoiceCallback
		//{
		//public:
		//	VoiceCallback();
		//	virtual ~VoiceCallback();
		//public:
		//	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32);
		//	STDMETHOD_(void, OnVoiceProcessingPassEnd)();
		//	STDMETHOD_(void, OnStreamEnd)();
		//	STDMETHOD_(void, OnBufferStart)(void*);
		//	STDMETHOD_(void, OnBufferEnd)(void*);
		//	STDMETHOD_(void, OnLoopEnd)(void*);
		//	STDMETHOD_(void, OnVoiceError)(void*, HRESULT);
		//private:
		//	TinyEvent	m_event;
		//};

		class TinyXAudio
		{
		public:
			TinyXAudio();
			virtual ~TinyXAudio();
			BOOL Initialize(const WAVEFORMATEX* pFMT);
		private:
			TinyComPtr<IXAudio2>				m_audio;
			IXAudio2MasteringVoice*				m_pMasteringVoice;
			IXAudio2SourceVoice*				m_pSourceVoice;
		};
	}
}
