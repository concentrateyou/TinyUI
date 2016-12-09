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
		BOOL TinyWASAPIAudio::GetDevices(vector<Name>& names)
		{
			TinyComPtr<IMMDeviceEnumerator>	enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDeviceCollection> collection;
			hRes = enumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &collection);
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
		BOOL TinyWASAPIAudio::GetDeviceFormats(const Name& name, AUDCLNT_SHAREMODE shareMode, vector<WAVEFORMATEX>& s)
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
			WAVEFORMATEX* pFMT = NULL;
			WAVEFORMATEX sFMT = { 0 };
			sFMT.wFormatTag = WAVE_FORMAT_PCM;
			sFMT.cbSize = 0;
			const UINT frequencys[7] = { 192000, 96000, 48000, 44100, 32000, 16000, 8000 };
			const UINT channels[2] = { 2,1 };
			const UINT bits[2] = { 16, 8 };
			for (UINT frequency = 0; frequency < ARRAYSIZE(frequencys); frequency++)
			{
				for (UINT channel = 0; channel < ARRAYSIZE(channels); channel++)
				{
					for (UINT bit = 0; bit < ARRAYSIZE(bits); bit++)
					{
						sFMT.wBitsPerSample = bits[bit];
						sFMT.nChannels = channels[channel];
						sFMT.nSamplesPerSec = frequencys[frequency];
						sFMT.nBlockAlign = sFMT.nChannels * sFMT.wBitsPerSample / 8;
						sFMT.nAvgBytesPerSec = sFMT.nSamplesPerSec * sFMT.nBlockAlign;
						hRes = audioClient->IsFormatSupported(shareMode, &sFMT, &pFMT);
						if (hRes == S_OK)
						{
							s.push_back(sFMT);
						}
					}
				}
			}
			return TRUE;
		}
	};
}
