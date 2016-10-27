#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "TinyMedia.h"
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <functiondiscoverykeys.h>

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// WAS�ӿ�
		/// </summary>
		/*class TinyWASAPI
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPI);
		public:
			class Name
			{
			public:
				Name();
				Name(string&& name, string&& id);
				Name(const string& name, const string& id);
				~Name();
				const string& name() const;
				const string& id() const;
			private:
				string	m_name;
				string	m_id;
			};
		public:
			TinyWASAPI();
			~TinyWASAPI();
		public:
			BOOL Initialize(const Name& name);
			BOOL Allocate(const AudioParameter& param);
			void Uninitialize();
		public:
			static BOOL GetDevices(EDataFlow dataFlow, vector<Name>& names);
		private:
			UINT							m_bufferSize;
			TinyComPtr<IMMDevice>			m_mmDevice;
			TinyComPtr<IAudioClient>		m_audioClient;
			TinyComPtr<IAudioClock>			m_audioClock;
			TinyComPtr<IAudioCaptureClient>	m_capture;
		};
		/// <summary>
		/// ��Ƶ��
		/// </summary>
		class TinyAudioStream
		{
		public:
			virtual BOOL Open() = 0;
			virtual void Start() = 0;
			virtual void Stop() = 0;
			virtual void SetVolume(double volume) = 0;
			virtual void GetVolume(double* volume) = 0;
			virtual void Close() = 0;
		};
		/// <summary>
		/// ������Ƶ��
		/// </summary>
		class TinyWASAPIAudioInputStream : public TinyAudioStream
		{
		public:
			TinyWASAPIAudioInputStream(const TinyWASAPI::Name& name, const AudioParameter& param);
		public:
			virtual BOOL Open() OVERRIDE;
			virtual void Start() OVERRIDE;
			virtual void Stop() OVERRIDE;
			virtual void SetVolume(double volume) OVERRIDE;
			virtual void GetVolume(double* volume) OVERRIDE;
			virtual void Close() OVERRIDE;
		private:
			TinyWASAPI	m_was;
		};
		/// <summary>
		/// �����Ƶ��
		/// </summary>
		class TinyWASAPIAudioOutputStream : public TinyAudioStream
		{
		public:
			TinyWASAPIAudioOutputStream(const TinyWASAPI::Name& name, const AudioParameter& param);
		public:
			virtual BOOL Open() OVERRIDE;
			virtual void Start() OVERRIDE;
			virtual void Stop() OVERRIDE;
			virtual void SetVolume(double volume) OVERRIDE;
			virtual void GetVolume(double* volume) OVERRIDE;
			virtual void Close() OVERRIDE;
		private:
			TinyWASAPI			m_was;
			AudioParameter		m_param;
			TinyWASAPI::Name	m_name;
		};*/
	};
}
