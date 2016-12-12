#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyWASAPIAudio : public TinyReference<TinyWASAPIAudio>, public IAudioSessionEvents, public IMMNotificationClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPIAudio)
		public:
			class Name
			{
			public:
				Name();
				Name(string&& id, string&& name);
				~Name();
				const string& name() const;
				const string& id() const;
			private:
				string		m_name;
				string		m_id;
			};
		public:
			TinyWASAPIAudio();
			virtual ~TinyWASAPIAudio();
		public:
			static BOOL GetDevices(EDataFlow dataFlow, vector<Name>& names);
			static BOOL GetDeviceFormats(const Name& name, AUDCLNT_SHAREMODE shareMode, vector<WAVEFORMATPCMEX>& s);
			static BOOL IsFormatValid(const Name& name, AUDCLNT_SHAREMODE shareMode, WAVEFORMATEX* pFMT);
		public:
			HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId);
			HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key) OVERRIDE;

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
	}
}
