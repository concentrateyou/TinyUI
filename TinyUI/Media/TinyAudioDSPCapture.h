#pragma once
#include "TinyWASAPIAudio.h"
#include "../IO/TinyTask.h"
#include <dmort.h>
#pragma comment(lib,"msdmo.lib")

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// DSP的音频捕获(支持麦克风阵列)
		/// SOURCE模式
		/// 目前支持一下输出格式
		/// 16000 1 16 WAVE_FORMAT_PCM
		/// </summary>
		class TinyAudioDSPCapture : public TinyWASAPIAudio, public AudioObserver
		{
			DISALLOW_COPY_AND_ASSIGN(TinyAudioDSPCapture)
		public:
			TinyAudioDSPCapture();
			virtual ~TinyAudioDSPCapture();
			void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			virtual	void Initialize();
			virtual	void Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback);
			virtual BOOL Open(const Name& speakName, const Name& captureName, WAVEFORMATEX* pFMT);
			virtual BOOL Open(const GUID& speakGUID, const GUID& captureGUID, WAVEFORMATEX* pFMT);
			virtual BOOL Start();
			virtual BOOL Stop();
			virtual BOOL Reset();
			virtual BOOL Close();
		public:
			BOOL			IsEmpty() const;
			BOOL			IsCapturing() const;
			WAVEFORMATEX*	GetFormat() const;
		public:
			void	EnableAGC(BOOL bAllow);
			void	EnableNS(BOOL bAllow);
			void	SetVADMode(AEC_VAD_MODE mode);//不支持Kinect audio
		private:
			void	OnMessagePump();
			BOOL	SetVTI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key, LONG value);
			BOOL	SetBOOLProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key, VARIANT_BOOL value);
		private:
			BOOL						m_bEnableAGC;
			BOOL						m_bEnableNS;
			BOOL						m_bCapturing;
			TinyEvent					m_audioStop;
			AEC_VAD_MODE				m_vadMode;
			DMO_MEDIA_TYPE				m_mediaType;
			DMO_OUTPUT_DATA_BUFFER		m_dmoBuffer;
			TinyScopedArray<BYTE>		m_waveFMT;
			IO::TinyTask			m_task;
			TinyComPtr<IMediaObject>	m_dmo;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	};
}
