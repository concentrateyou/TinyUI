#pragma once
#include "TinyMedia.h"
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xapofx.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
#pragma comment(lib,"x3daudio.lib")
#pragma comment(lib,"xapofx.lib")

namespace TinyUI
{
	namespace Media
	{
		class VoiceCallback : public IXAudio2VoiceCallback
		{
		public:
			VoiceCallback();
			virtual ~VoiceCallback();
		public:
			STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32);
			STDMETHOD_(void, OnVoiceProcessingPassEnd)();
			STDMETHOD_(void, OnStreamEnd)();
			STDMETHOD_(void, OnBufferStart)(void*);
			STDMETHOD_(void, OnBufferEnd)(void*);
			STDMETHOD_(void, OnLoopEnd)(void*);
			STDMETHOD_(void, OnVoiceError)(void*, HRESULT);
		private:
			TinyEvent	m_event;
		};
		/// <summary>
		/// “Ù∆µ≤•∑≈
		/// </summary>
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
			XAUDIO2_BUFFER						m_block[3];
		};
	}
}
