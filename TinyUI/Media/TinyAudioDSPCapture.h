#pragma once
#include "TinyWASAPIAudio.h"
#include "../IO/TinyTaskBase.h"
#include <dmort.h>
#pragma comment(lib,"msdmo.lib")

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// DSP的音频捕获(只支持麦克风阵列)
		/// </summary>
		class TinyAudioDSPCapture : public TinyWASAPIAudio, public AudioObserver
		{
			DISALLOW_COPY_AND_ASSIGN(TinyAudioDSPCapture)
		public:
			TinyAudioDSPCapture();
			virtual ~TinyAudioDSPCapture();
			void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			virtual	void Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback);
			virtual BOOL Open(const Name& speakName, const Name& captureName, WAVEFORMATEX* pFMT);
			virtual BOOL Start();
			virtual BOOL Stop();
			virtual BOOL Reset();
			virtual BOOL Close();
			WAVEFORMATEX* GetFormat() const;
		public:
			void EnableAGC(BOOL bAllow);
			void EnableNS(BOOL bAllow);
		private:
			void OnMessagePump();
			BOOL SetVTI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key, LONG value);
			BOOL SetBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key, VARIANT_BOOL value);
		private:
			BOOL						m_bEnableAGC;
			BOOL						m_bEnableNS;
			BOOL						m_bCapturing;
			WAVEFORMATEX*				m_pFMT;
			DMO_MEDIA_TYPE				m_mediaType;
			TinyEvent					m_audioStop;
			IO::TinyTaskBase			m_task;
			TinyComPtr<IMediaObject>	m_dmo;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	};
}
