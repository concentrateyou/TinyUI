#include "stdafx.h"
#include "VideoCaptureDevice.h"
#include "Common/TinyString.h"

namespace Media
{
	GUID MediaSubTypeI420 =
	{
		0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }
	};
	GUID MediaSubTypeHDYC =
	{
		0x43594448, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
	};
	ScopedMediaType::ScopedMediaType()
		:m_mediaType(NULL)
	{

	}
	ScopedMediaType::~ScopedMediaType()
	{
		Release();
	}
	AM_MEDIA_TYPE* ScopedMediaType::operator->()
	{
		return m_mediaType;
	}
	AM_MEDIA_TYPE* ScopedMediaType::Ptr()
	{
		return m_mediaType;
	}
	void ScopedMediaType::Release()
	{
		if (m_mediaType)
		{
			if (m_mediaType->cbFormat != NULL)
			{
				CoTaskMemFree(m_mediaType->pbFormat);
				m_mediaType->cbFormat = 0;
				m_mediaType->pbFormat = NULL;
			}
			if (m_mediaType->pUnk != NULL)
			{
				m_mediaType->pUnk->Release();
				m_mediaType->pUnk = NULL;
			}
			m_mediaType = NULL;
		}
	}
	AM_MEDIA_TYPE** ScopedMediaType::Receive()
	{
		return &m_mediaType;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoCaptureDevice::Name::Name(const string& name, const string& id)
		:m_name(name),
		m_id(id)
	{

	}
	VideoCaptureDevice::Name::~Name()
	{

	}
	const string& VideoCaptureDevice::Name::name() const
	{
		return m_name;
	}
	const string& VideoCaptureDevice::Name::id() const
	{
		return m_id;
	}
	VideoCaptureDevice::VideoCaptureDevice(const string& name)

	{

	}
	VideoCaptureDevice::~VideoCaptureDevice()
	{

	}
	BOOL VideoCaptureDevice::Initialize()
	{
		return TRUE;
	}
	BOOL VideoCaptureDevice::GetDevices(vector<Name>& names)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (FAILED(hRes))
			return FALSE;
		names.clear();
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (FAILED(hRes))
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"Description", &variant, 0);
			if (FAILED(hRes))
			{
				hRes = propertyBag->Read(L"FriendlyName", &variant, 0);
			}
			if (SUCCEEDED(hRes) && variant->vt == VT_BSTR)
			{
				const WCHAR* str = V_BSTR(&variant);
				string id;
				string name(WStringToUTF8(str));
				variant.Reset();
				hRes = propertyBag->Read(L"DevicePath", &variant, 0);
				if (FAILED(hRes) || variant->vt != VT_BSTR)
				{
					id = name;
				}
				else
				{
					id = WStringToUTF8(V_BSTR(&variant));
				}
				names.push_back(Name(name, id));
			}
			moniker.Release();
		}
	}
	BOOL VideoCaptureDevice::GetDeviceFilter(const Name& name, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> dev;
		HRESULT hRes = dev.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		DWORD fetched = 0;
		while (enumMoniker->Next(1, &moniker, &fetched) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (FAILED(hRes))
			{
				moniker.Release();
				continue;
			}
			static const WCHAR* PropertyNames[] =
			{
				L"DevicePath",
				L"Description",
				L"FriendlyName"
			};
			ScopedVariant variant;
			for (size_t i = 0; i < arraysize(PropertyNames) && variant->vt != VT_BSTR; ++i)
			{
				propertyBag->Read(PropertyNames[i], &variant, 0);
			}
			if (variant->vt == VT_BSTR)
			{
				string device = UTF16ToUTF8(V_BSTR(variant));
				if (device.compare(name.name()) == 0)
				{
					hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ps);
					if (SUCCEEDED(hRes) && ps)
					{
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}
	BOOL VideoCaptureDevice::GetCategory(IPin* pin, REFGUID category)
	{
		BOOL bFlag = FALSE;
		TinyComPtr<IKsPropertySet> ksProperty;
		HRESULT hRes = ksProperty->QueryInterface(&pin);
		if (SUCCEEDED(hRes))
		{
			GUID category1;
			DWORD dwValue;
			hRes = ksProperty->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, &category1, sizeof(category1), &dwValue);
			if (SUCCEEDED(hRes) && (dwValue == sizeof(category1)))
			{
				bFlag = (category1 == category);
			}
		}
		return bFlag;
	}
	TinyComPtr<IPin> VideoCaptureDevice::GetPin(IBaseFilter* filter, PIN_DIRECTION pinDIR, REFGUID category)
	{
		TinyComPtr<IPin> pin;
		TinyComPtr<IEnumPins> enumPins;
		HRESULT hRes = filter->EnumPins(&enumPins);
		if (enumPins == NULL)
			return pin;
		hRes = enumPins->Reset();
		while ((hRes = enumPins->Next(1, &pin, NULL)) == S_OK)
		{
			PIN_DIRECTION pd = static_cast<PIN_DIRECTION>(-1);
			hRes = pin->QueryDirection(&pd);
			if (pinDIR == pd)
			{
				if (category == GUID_NULL || GetCategory(pin, category))
					return pin;
			}
			pin.Release();
		}
		return pin;
	}
	VideoPixelFormat VideoCaptureDevice::TranslateMediaSubtypeToPixelFormat(const GUID& subType)
	{
		static struct
		{
			const GUID& subType;
			VideoPixelFormat format;
		}
		formats[] =
		{
			{ MediaSubTypeI420, PIXEL_FORMAT_I420 },
			{ MEDIASUBTYPE_IYUV, PIXEL_FORMAT_I420 },
			{ MEDIASUBTYPE_RGB24, PIXEL_FORMAT_RGB24 },
			{ MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2 },
			{ MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG },
			{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY },
			{ MEDIASUBTYPE_ARGB32, PIXEL_FORMAT_ARGB },
			{ MediaSubTypeHDYC, PIXEL_FORMAT_UYVY },
		};
		for (size_t i = 0; i < ARRAYSIZE_UNSAFE(formats); ++i)
		{
			if (subType == formats[i].subType)
				return formats[i].format;
		}
		WCHAR str[128];
		StringFromGUID2(subType, str, arraysize(str));
		TRACE("%s", UTF16ToUTF8(str).c_str());
		return PIXEL_FORMAT_UNKNOWN;
	}
}
