#pragma once
#include "TinyMedia.h"
#include <objbase.h>
#include <mmreg.h>
#include <audioclient.h>
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#if defined(_MSC_VER) && (_MSC_VER < 1700)
#error DirectX Tool Kit for Audio does not support VS 2010 without the DirectX SDK 
#endif
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>
#pragma comment(lib,"xaudio2.lib")
#else
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xapofx.h>
#pragma warning(push)
#pragma warning( disable : 4005 )
#include <D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
#pragma warning(pop)
#pragma comment(lib,"x3daudio.lib")
#pragma comment(lib,"xapofx.lib")
#endif

namespace TinyUI
{
	namespace Media
	{
#define MAX_BUFFER_COUNT 3

		class VoiceCallback : public IXAudio2VoiceCallback
		{
		public:
			VoiceCallback(Closure&& callback);
			virtual ~VoiceCallback();
		public:
			BOOL Lock(DWORD dwMS);
			void Unlock();
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
			Closure		m_callback;
		};
		/// <summary>
		/// “Ù∆µ≤•∑≈
		/// </summary>
		class TinyXAudio
		{
		public:
			TinyXAudio();
			virtual ~TinyXAudio();
			BOOL Open(const WAVEFORMATEX* pFMT,Closure&& callback);
			BOOL Fill(BYTE* bits, LONG size);
			BOOL Start();
			BOOL Stop();
			BOOL Close();
		private:
			void OnVoice();
		private:
			Closure								m_callback;
			TinyScopedArray<BYTE>				m_bits;
			TinyComPtr<IXAudio2>				m_audio;
			IXAudio2MasteringVoice*				m_pMasteringVoice;
			IXAudio2SourceVoice*				m_pSourceVoice;
			VoiceCallback						m_voiceCallback;
		};
	}
}
