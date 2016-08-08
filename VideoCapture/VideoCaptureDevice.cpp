#include "stdafx.h"
#include "VideoCaptureDevice.h"
#include "Common/TinyString.h"

namespace VideoCapture
{
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
	VideoCaptureDevice::VideoCaptureDevice(const string& name)

	{

	}
	VideoCaptureDevice::~VideoCaptureDevice()
	{

	}
	BOOL VideoCaptureDevice::Initialize()
	{

	}

	BOOL VideoCaptureDevice::GetDeviceFilter(const string& name, IBaseFilter** ps)
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
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, &propertyBag);
			if (FAILED(hRes))
			{
				continue;
			}
			static const WCHAR* PropertyNames[] =
			{
				L"DevicePath",
				L"Description",
				L"FriendlyName"
			};
			VARIANT var;
			VariantInit(&var);
			for (size_t i = 0; i < arraysize(PropertyNames) && var.vt != VT_BSTR; ++i)
			{
				propertyBag->Read(PropertyNames[i], &var, 0);
			}
			if (var.vt == VT_BSTR)
			{
				string device = UTF16ToUTF8(V_BSTR(&var));
				if (device.compare(name) == 0)
				{
					hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ps);
					if (SUCCEEDED(hRes) && ps)
					{
						return TRUE;
					}
				}
			}
			VariantClear(&var);
		}
		return TRUE;
	}
}
