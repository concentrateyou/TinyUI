#include "stdafx.h"
#include "AudioManager.h"

namespace MediaSDK
{
	AudioManager::AudioManager()
	{

	}

	AudioManager::~AudioManager()
	{
	}

	BOOL AudioManager::Initialize()
	{
		return m_listener.Initialize(BindCallback(&AudioManager::OnChange, this));
	}

	void AudioManager::OnChange(LPCWSTR dID, DWORD val)
	{

	}

	void AudioManager::Add(AudioInputStream* stream)
	{
		m_inputs.Add(stream);
	}
	void AudioManager::Add(AudioOutputStream* stream)
	{
		m_outputs.Add(stream);
	}
	void AudioManager::Remove(AudioInputStream* stream)
	{
		m_inputs.Remove(stream);
	}
	void AudioManager::Remove(AudioOutputStream* stream)
	{
		m_outputs.Remove(stream);
	}
	IMMDevice* AudioManager:: GetDefaultDevice(EDataFlow dataFlow, ERole role)
	{
		TinyComPtr<IMMDeviceEnumerator> enumerator;
		HRESULT hRes = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		if (hRes == CO_E_NOTINITIALIZED)
		{
			hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			if (SUCCEEDED(hRes))
			{
				hRes = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
			}
		}
		if (FAILED(hRes))
			return NULL;
		IMMDevice* device = NULL;
		hRes = enumerator->GetDefaultAudioEndpoint(dataFlow, role, &device);
		if (FAILED(hRes))
			return NULL;
		return device;
	}
	string AudioManager::GetDeviceID(EDataFlow dataFlow, ERole role)
	{
		string deviceID;
		TinyComPtr<IMMDevice> device( GetDefaultDevice(dataFlow, role));
		if (device != NULL)
		{
			WCHAR* wID = NULL;
			if (SUCCEEDED(device->GetId(&wID)))
			{
				deviceID = UTF16ToUTF8(wID);
				CoTaskMemFree(wID);
			}
		}
		return deviceID;
	}
	string AudioManager::GetDefaultInputID()
	{
		return GetDeviceID(eCapture, eConsole);
	}
	string AudioManager::GetDefaultOutputID()
	{
		return GetDeviceID(eRender, eConsole);
	}
	string AudioManager::GetCommunicationsInputID()
	{
		return GetDeviceID(eCapture, eCommunications);
	}
	string AudioManager::GetCommunicationsOutputID()
	{
		return GetDeviceID(eRender, eCommunications);
	}
}