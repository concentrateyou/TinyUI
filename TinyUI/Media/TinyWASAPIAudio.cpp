#include "../stdafx.h"
#include "TinyWASAPIAudio.h"

namespace TinyUI
{
	namespace Media
	{
		AudioSessionEvents::AudioSessionEvents()
		{

		}
		AudioSessionEvents::~AudioSessionEvents()
		{

		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnDisplayNameChanged(_In_ LPCWSTR NewDisplayName, LPCGUID EventContext)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnIconPathChanged(_In_ LPCWSTR NewIconPath, LPCGUID EventContext)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnSimpleVolumeChanged(_In_ float NewVolume, _In_ BOOL NewMute, LPCGUID EventContext)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnChannelVolumeChanged(_In_ DWORD ChannelCount, _In_reads_(ChannelCount) float NewChannelVolumeArray[], _In_ DWORD ChangedChannel, LPCGUID EventContext)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnGroupingParamChanged(_In_ LPCGUID NewGroupingParam, LPCGUID EventContext)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnStateChanged(_In_ AudioSessionState NewState)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnSessionDisconnected(_In_ AudioSessionDisconnectReason DisconnectReason)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioSessionEvents::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IAudioSessionEvents)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IAudioSessionEvents*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}
		ULONG STDMETHODCALLTYPE AudioSessionEvents::AddRef(void)
		{
			TinyReference < AudioSessionEvents >::AddRef();
			return TinyReference < AudioSessionEvents >::GetReference();
		}
		ULONG STDMETHODCALLTYPE AudioSessionEvents::Release(void)
		{
			TinyReference < AudioSessionEvents >::Release();
			return TinyReference < AudioSessionEvents >::GetReference();
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWASAPIAudio::Name::Name()
		{

		}
		TinyWASAPIAudio::Name::Name(string&& id, string&& name)
			: m_name(std::move(name)),
			m_id(std::move(id))
		{

		}
		TinyWASAPIAudio::Name::~Name()
		{

		}
		const string& TinyWASAPIAudio::Name::name() const
		{
			return m_name;
		}
		const string& TinyWASAPIAudio::Name::id() const
		{
			return m_id;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWASAPIAudio::TinyWASAPIAudio()
		{

		}
		TinyWASAPIAudio::~TinyWASAPIAudio()
		{

		}
		BOOL TinyWASAPIAudio::GetDevices(EDataFlow dataFlow, vector<Name>& names)
		{
			TinyComPtr<IMMDeviceEnumerator>	enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMMDeviceCollection> collection;
			hRes = enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &collection);
			if (hRes != S_OK)
				return FALSE;
			UINT count = 0;
			hRes = collection->GetCount(&count);
			if (hRes != S_OK)
				return FALSE;
			for (UINT i = 0;i < count;i++)
			{
				TinyComPtr<IMMDevice> device;
				hRes = collection->Item(i, &device);
				if (hRes != S_OK)
					continue;
				string id;
				LPWSTR pwszVal = NULL;
				hRes = device->GetId(&pwszVal);
				if (SUCCEEDED(hRes) && pwszVal != NULL)
				{
					id = WStringToString(pwszVal);
					CoTaskMemFree(pwszVal);
					pwszVal = NULL;
				}
				string name;
				TinyComPtr<IPropertyStore> prop;
				hRes = device->OpenPropertyStore(STGM_READ, &prop);
				if (SUCCEEDED(hRes))
				{
					PROPVARIANT varName;
					PropVariantInit(&varName);
					hRes = prop->GetValue(PKEY_Device_FriendlyName, &varName);
					if (SUCCEEDED(hRes) && varName.pwszVal != NULL)
					{
						name = WStringToString(varName.pwszVal);
						names.push_back(Name(std::move(id), std::move(name)));
						PropVariantClear(&varName);
					}
				}
			}
			return TRUE;
		}
		BOOL TinyWASAPIAudio::IsFormatValid(const Name& name, AUDCLNT_SHAREMODE shareMode, WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IMMDeviceEnumerator>	enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IAudioClient> audioClient;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&audioClient);
			if (hRes != S_OK)
				return FALSE;
			WAVEFORMATEX* ps = NULL;
			hRes = audioClient->IsFormatSupported(shareMode, pFMT, &ps);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
	};
}
