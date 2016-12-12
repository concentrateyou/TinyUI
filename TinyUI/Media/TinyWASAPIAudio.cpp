#include "../stdafx.h"
#include "TinyWASAPIAudio.h"

namespace TinyUI
{
	namespace Media
	{
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
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDeviceCollection> collection;
			hRes = enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &collection);
			if (FAILED(hRes))
				return FALSE;
			UINT count = 0;
			hRes = collection->GetCount(&count);
			if (FAILED(hRes))
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
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IAudioClient> audioClient;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&audioClient);
			if (FAILED(hRes))
				return FALSE;
			WAVEFORMATEX* ps = NULL;
			return audioClient->IsFormatSupported(shareMode, pFMT, &ps) == S_OK;
		}
		BOOL TinyWASAPIAudio::GetDeviceFormats(const Name& name, AUDCLNT_SHAREMODE shareMode, vector<WAVEFORMATPCMEX>& s)
		{
			TinyComPtr<IMMDeviceEnumerator>	enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IAudioClient> audioClient;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&audioClient);
			if (FAILED(hRes))
				return FALSE;
			WAVEFORMATPCMEX* ps = NULL;
			hRes = audioClient->GetMixFormat(reinterpret_cast<WAVEFORMATEX**>(&ps));
			if (FAILED(hRes))
				return FALSE;
			WAVEFORMATEX* pFMT = NULL;
			WAVEFORMATPCMEX sFMT = { 0 };
			const UINT frequencys[7] = { 192000, 96000, 48000, 44100, 32000, 16000, 8000 };
			const UINT channels[2] = { 2,1 };
			const UINT bits[4] = { 32, 24, 16, 8 };
			for (UINT frequency = 0; frequency < ARRAYSIZE(frequencys); frequency++)
			{
				for (UINT channel = 0; channel < ARRAYSIZE(channels); channel++)
				{
					for (UINT bit = 0; bit < ARRAYSIZE(bits); bit++)
					{
						sFMT.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
						sFMT.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
						sFMT.Format.wBitsPerSample = bits[bit];
						sFMT.Format.nChannels = channels[channel];
						sFMT.Format.nSamplesPerSec = frequencys[frequency];
						sFMT.Format.nBlockAlign = sFMT.Format.nChannels * sFMT.Format.wBitsPerSample / 8;
						sFMT.Format.nAvgBytesPerSec = sFMT.Format.nSamplesPerSec * sFMT.Format.nBlockAlign;
						sFMT.Samples.wValidBitsPerSample = bits[bit];
						sFMT.dwChannelMask = (sizeof(WAVEFORMATEX) + ps->Format.cbSize) == sizeof(WAVEFORMATPCMEX) ? ps->dwChannelMask : 0;
						sFMT.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
						hRes = audioClient->IsFormatSupported(shareMode, (WAVEFORMATEX*)&sFMT, &pFMT);
						if (hRes == S_OK)
						{
							s.push_back(sFMT);
						}
					}
				}
			}
			return TRUE;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnDisplayNameChanged(_In_ LPCWSTR NewDisplayName, LPCGUID EventContext)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnIconPathChanged(_In_ LPCWSTR NewIconPath, LPCGUID EventContext)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnSimpleVolumeChanged(_In_ float NewVolume, _In_ BOOL NewMute, LPCGUID EventContext)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnChannelVolumeChanged(_In_ DWORD ChannelCount, _In_reads_(ChannelCount) float NewChannelVolumeArray[], _In_ DWORD ChangedChannel, LPCGUID EventContext)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnGroupingParamChanged(_In_ LPCGUID NewGroupingParam, LPCGUID EventContext)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnStateChanged(_In_ AudioSessionState NewState)
		{
			return S_OK;
		}


		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnSessionDisconnected(_In_ AudioSessionDisconnectReason DisconnectReason)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE TinyWASAPIAudio::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IAudioSessionEvents)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IAudioSessionEvents*>(this);
			}
			else if (IsEqualIID(riid, __uuidof(IMMNotificationClient)))
			{
				*ppvObject = static_cast<IMMNotificationClient*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}


		ULONG STDMETHODCALLTYPE TinyWASAPIAudio::AddRef(void)
		{
			TinyReference < TinyWASAPIAudio >::AddRef();
			return TinyReference < TinyWASAPIAudio >::GetReference();
		}


		ULONG STDMETHODCALLTYPE TinyWASAPIAudio::Release(void)
		{
			TinyReference < TinyWASAPIAudio >::Release();
			return TinyReference < TinyWASAPIAudio >::GetReference();
		}
	};
}
