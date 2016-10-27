#include "../stdafx.h"
#include "TinyWASAPI.h"

namespace TinyUI
{
	namespace Media
	{
		//TinyWASAPI::Name::Name()
		//{

		//}
		//TinyWASAPI::Name::Name(const string& name, const string& id)
		//	:m_name(name),
		//	m_id(id)
		//{

		//}
		//TinyWASAPI::Name::Name(string&& name, string&& id)
		//	: m_name(std::move(name)),
		//	m_id(std::move(id))
		//{

		//}
		//TinyWASAPI::Name::~Name()
		//{

		//}
		//const string& TinyWASAPI::Name::name() const
		//{
		//	return m_name;
		//}
		//const string& TinyWASAPI::Name::id() const
		//{
		//	return m_id;
		//}
		//TinyWASAPI::TinyWASAPI()
		//	:m_bufferSize(0)
		//{

		//}
		//TinyWASAPI::~TinyWASAPI()
		//{

		//}
		//BOOL TinyWASAPI::Initialize(const Name& name)
		//{
		//	TinyComPtr<IMMDeviceEnumerator> enumerator;
		//	HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
		//	if (FAILED(hRes))
		//	{
		//		return FALSE;
		//	}
		//	EDataFlow dataFlow = EDataFlow::eAll;
		//	TinyComPtr<IMMDeviceCollection>	collection;
		//	hRes = enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &collection);
		//	if (FAILED(hRes))
		//	{
		//		return FALSE;
		//	}
		//	UINT count = 0;
		//	collection->GetCount(&count);
		//	if (count == 0)
		//		return FALSE;
		//	for (UINT i = 0; i < count; ++i)
		//	{
		//		m_mmDevice.Release();
		//		hRes = collection->Item(i, &m_mmDevice);
		//		if (FAILED(hRes))
		//			continue;
		//		TinyComPtr<IPropertyStore> properties;
		//		hRes = m_mmDevice->OpenPropertyStore(STGM_READ, &properties);
		//		if (FAILED(hRes))
		//			continue;
		//		PROPVARIANT prop;
		//		PropVariantInit(&prop);
		//		hRes = properties->GetValue(PKEY_Device_FriendlyName, &prop);
		//		if (FAILED(hRes))
		//			continue;
		//		if (prop.vt = VT_LPWSTR && prop.pwszVal)
		//		{
		//			string val = WStringToString(prop.pwszVal);
		//			if (val == name.name())
		//			{
		//				return TRUE;
		//			}
		//		}
		//		return FALSE;
		//	}
		//}
		//void TinyWASAPI::Uninitialize()
		//{

		//}
		//BOOL TinyWASAPI::Allocate(const AudioParameter& param)
		//{
		//	m_audioClient.Release();
		//	HRESULT hRes = m_mmDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_audioClient);
		//	if (FAILED(hRes))
		//		return FALSE;
		//	WAVEFORMATEX* closest = NULL;
		//	WAVEFORMATEX ws = param.GetFormat();
		//	hRes = m_audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &ws, &closest);
		//	if (FAILED(hRes))
		//		return FALSE;
		//	CoTaskMemFree(closest);
		//	closest = NULL;
		//	hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, 0, 0, &ws, NULL);
		//	if (FAILED(hRes))
		//		return FALSE;
		//	hRes = m_audioClient->GetBufferSize(&m_bufferSize);
		//	if (FAILED(hRes))
		//		return FALSE;
		//	return TRUE;
		//}
		//BOOL TinyWASAPI::GetDevices(EDataFlow dataFlow, vector<Name>& names)
		//{
		//	TinyComPtr<IMMDeviceEnumerator> enumerator;
		//	HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
		//	if (FAILED(hRes))
		//	{
		//		return FALSE;
		//	}
		//	TinyComPtr<IMMDeviceCollection>	collection;
		//	hRes = enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &collection);
		//	if (FAILED(hRes))
		//	{
		//		return FALSE;
		//	}
		//	UINT count = 0;
		//	collection->GetCount(&count);
		//	if (count == 0)
		//		return FALSE;
		//	for (UINT i = 0; i < count; ++i)
		//	{
		//		TinyComPtr<IMMDevice> device;
		//		hRes = collection->Item(i, &device);
		//		if (FAILED(hRes))
		//			continue;
		//		TinyComPtr<IPropertyStore> properties;
		//		hRes = device->OpenPropertyStore(STGM_READ, &properties);
		//		if (FAILED(hRes))
		//			continue;
		//		PROPVARIANT val;
		//		hRes = properties->GetValue(PKEY_Device_FriendlyName, &val);
		//		if (FAILED(hRes))
		//			continue;
		//		if (val.vt = VT_LPWSTR && val.pwszVal)
		//		{
		//			string name = WStringToString(val.pwszVal);
		//			WCHAR* wID = NULL;
		//			hRes = device->GetId(&wID);
		//			if (SUCCEEDED(hRes) && wID)
		//			{
		//				string id = WStringToString(wID);
		//				names.push_back(std::move(Name(std::move(name), std::move(id))));
		//				CoTaskMemFree(wID);
		//				wID = NULL;
		//			}
		//		}
		//	}
		//	return TRUE;
		//};
		////////////////////////////////////////////////////////////////////////////
		//BOOL TinyWASAPIAudioInputStream::Open()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioInputStream::Start()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioInputStream::Stop()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioInputStream::SetVolume(double volume)
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioInputStream::GetVolume(double* volume)
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioInputStream::Close()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}
		////////////////////////////////////////////////////////////////////////////
		//TinyWASAPIAudioOutputStream::TinyWASAPIAudioOutputStream(const TinyWASAPI::Name& name, const AudioParameter& param)
		//	:m_name(name),
		//	m_param(param)
		//{

		//}
		//BOOL TinyWASAPIAudioOutputStream::Open()
		//{
		//	if (!m_was.Initialize(m_name, m_param.GetFormat()))
		//		return FALSE;

		//	return TRUE;
		//}

		//void TinyWASAPIAudioOutputStream::Start()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioOutputStream::Stop()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioOutputStream::SetVolume(double volume)
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioOutputStream::GetVolume(double* volume)
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}

		//void TinyWASAPIAudioOutputStream::Close()
		//{
		//	throw std::logic_error("The method or operation is not implemented.");
		//}
	}
}
