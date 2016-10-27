#include "../stdafx.h"
#include "TinyWASAPI.h"

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPI::Name::Name()
		{

		}
		TinyWASAPI::Name::Name(const string& name, const string& id)
			:m_name(name),
			m_id(id)
		{

		}
		TinyWASAPI::Name::Name(string&& name, string&& id)
			: m_name(std::move(name)),
			m_id(std::move(id))
		{

		}
		TinyWASAPI::Name::~Name()
		{

		}
		const string& TinyWASAPI::Name::name() const
		{
			return m_name;
		}
		const string& TinyWASAPI::Name::id() const
		{
			return m_id;
		}
		TinyWASAPI::TinyWASAPI()
		{

		}
		TinyWASAPI::~TinyWASAPI()
		{

		}
		BOOL TinyWASAPI::GetDevices(vector<Name>& names)
		{
			TinyComPtr<IMMDeviceEnumerator> m_enumerator;
			HRESULT hRes = m_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
			{
				return FALSE;
			}
			EDataFlow dataFlow = EDataFlow::eAll;
			TinyComPtr<IMMDeviceCollection>	m_collection;
			hRes = m_enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &m_collection);
			if (FAILED(hRes))
			{
				return FALSE;
			}
			UINT count = 0;
			m_collection->GetCount(&count);
			if (count == 0)
				return FALSE;
			for (UINT i = 0; i < count; ++i)
			{
				TinyComPtr<IMMDevice> device;
				hRes = m_collection->Item(i, &device);
				if (FAILED(hRes))
					continue;
				TinyComPtr<IPropertyStore> properties;
				hRes = device->OpenPropertyStore(STGM_READ, &properties);
				if (FAILED(hRes))
					continue;
				PROPVARIANT val;
				hRes = properties->GetValue(PKEY_Device_FriendlyName, &val);
				if (FAILED(hRes))
					continue;
				if (val.vt = VT_LPWSTR && val.pwszVal)
				{
					string name = WStringToString(val.pwszVal);
					WCHAR* wID = NULL;
					hRes = device->GetId(&wID);
					if (SUCCEEDED(hRes) && wID)
					{
						string id = WStringToString(wID);
						names.push_back(std::move(Name(std::move(name), std::move(id))));
						CoTaskMemFree(wID);
						wID = NULL;
					}
				}
			}
			return TRUE;
		};
	}
}
