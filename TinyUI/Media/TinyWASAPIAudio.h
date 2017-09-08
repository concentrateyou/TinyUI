#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		class AudioSessionEvents : public TinyReference<AudioSessionEvents>, public IAudioSessionEvents
		{
		public:
			AudioSessionEvents();
			virtual ~AudioSessionEvents();
		public:
			HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(_In_ LPCWSTR NewDisplayName, LPCGUID EventContext) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnIconPathChanged(_In_ LPCWSTR NewIconPath, LPCGUID EventContext) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(_In_ float NewVolume, _In_ BOOL NewMute, LPCGUID EventContext) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(_In_ DWORD ChannelCount, _In_reads_(ChannelCount) float NewChannelVolumeArray[], _In_ DWORD ChangedChannel, LPCGUID EventContext) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(_In_ LPCGUID NewGroupingParam, LPCGUID EventContext) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnStateChanged(_In_ AudioSessionState NewState) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnSessionDisconnected(_In_ AudioSessionDisconnectReason DisconnectReason) OVERRIDE;
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
		};
		class TinyWASAPIAudio
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPIAudio)
		public:
			class Name
			{
			public:
				Name();
				Name(const string& id, const string& name, const GUID& type, const GUID& subType);
				Name(string&& id, string&& name, const GUID& type, const GUID& subType);
				~Name();
				BOOL IsEmpty() const;
				const string& name() const;
				const string& id() const;
				const GUID& type() const;
				const GUID& subtype() const;
			private:
				string		m_name;
				string		m_id;
				GUID		m_type;
				GUID		m_subType;
			};
		public:
			TinyWASAPIAudio();
			virtual ~TinyWASAPIAudio();
		public:
			static BOOL IsMicrophoneArray(const string& name, BOOL& IsMA);
			static BOOL IsMicrophoneArray(const GUID& guid, BOOL& IsMA);
			static BOOL IsMicrophoneArray(const Name& name, BOOL& IsMA);
			static BOOL IsMicrophone(const string& name, BOOL& IsMA);
			static BOOL IsMicrophone(const GUID& guid, BOOL& IsMA);
			static BOOL IsMicrophone(const Name& name, BOOL& IsMA);
			static BOOL GetDevices(EDataFlow dataFlow, vector<Name>& names);
			static INT  GetDeviceIndex(EDataFlow dataFlow, const Name& name);
			static INT  GetDeviceIndex(EDataFlow dataFlow, const GUID& guid);
			static BOOL GetJackSubtype(IMMDevice* mmDevice, GUID& subType);
			static BOOL IsFormatValid(const Name& name, AUDCLNT_SHAREMODE shareMode, WAVEFORMATEX* pFMT);
		};
	}
}
