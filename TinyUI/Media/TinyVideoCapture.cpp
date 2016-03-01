#include "../stdafx.h"
#include "TinyVideoCapture.h"

namespace TinyUI
{
	namespace Media
	{
		TinyVideoCapture::TinyVideoCapture()
		{

		}
		TinyVideoCapture::~TinyVideoCapture()
		{

		}
		BOOL TinyVideoCapture::Initialize()
		{
			HRESULT hRes = S_OK;
			hRes = CoCreateInstance(
				CLSID_FilterGraph,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IGraphBuilder,
				(void**)&m_graphBuilder);
			if (FAILED(hRes)) return FALSE;
			hRes = m_graphBuilder->QueryInterface(IID_IMediaControl, (void**)&m_mediaControl);
			if (FAILED(hRes)) return FALSE;
			hRes = m_graphBuilder->QueryInterface(IID_IMediaEvent, (void**)&m_mediaEvent);
			if (FAILED(hRes)) return FALSE;

			return TRUE;
		}
		BOOL TinyVideoCapture::Uninitialize()
		{
			return TRUE;
		}
		BOOL TinyVideoCapture::GetDeviceNames(TinyArray<TinyString>& names)
		{
			TinyComPtr<ICreateDevEnum> devs;
			HRESULT hRes = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&devs);
			if (FAILED(hRes)) return FALSE;
			TinyComPtr<IEnumMoniker> monikers;
			hRes = devs->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &monikers, 0);
			if (FAILED(hRes)) return FALSE;
			monikers->Reset();
			TinyComPtr<IMoniker> moniker;
			ULONG cFetched = 0;
			BOOL success = TRUE;
			while (monikers->Next(1, &moniker, &cFetched) == S_OK)
			{
				TinyComPtr<IPropertyBag> bag;
				hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&bag);
				if (FAILED(hRes))
				{
					success = FALSE;
					goto ERROR_LABEL;
				}
				VARIANT val;
				hRes = bag->Read(L"Description", &val, 0);
				if (FAILED(hRes))
				{
					hRes = bag->Read(L"FriendlyName", &val, 0);
				}
				if (FAILED(hRes) || val.vt != VT_BSTR)
				{
					success = FALSE;
					goto ERROR_LABEL;
				}
				names.Add(UTF16ToUTF8(V_BSTR(&val)).c_str());
			ERROR_LABEL:
				moniker.Release();
			}
			return success;
		}
		BOOL TinyVideoCapture::GetDeviceFilter(const string& name, IBaseFilter** ps)
		{
			TinyComPtr<ICreateDevEnum> devs;
			HRESULT hRes = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&devs);
			if (FAILED(hRes)) return FALSE;
			TinyComPtr<IEnumMoniker> monikers;
			hRes = devs->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &monikers, 0);
			if (FAILED(hRes)) return FALSE;
			monikers->Reset();
			TinyComPtr<IMoniker> moniker;
			ULONG cFetched = 0;
			BOOL success = TRUE;
			while (monikers->Next(1, &moniker, &cFetched) == S_OK)
			{
				TinyComPtr<IPropertyBag> bag;
				hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&bag);
				if (FAILED(hRes))
				{
					success = FALSE;
					goto ERROR_LABEL;
				}
				VARIANT val;
				hRes = bag->Read(L"Description", &val, 0);
				if (FAILED(hRes))
				{
					hRes = bag->Read(L"FriendlyName", &val, 0);
				}
				if (FAILED(hRes) || val.vt != VT_BSTR)
				{
					success = FALSE;
					goto ERROR_LABEL;
				}
				if (name == UTF16ToUTF8(V_BSTR(&val)))
				{
					hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)ps);
					if (FAILED(hRes))
					{
						success = FALSE;
						goto ERROR_LABEL;
					}
				}
			ERROR_LABEL:
				moniker.Release();
			}
			return success;
		}
		void TinyVideoCapture::EnumFilter(Callback<BOOL(LPCSTR, IBaseFilter**)>& callback)
		{
			TinyComPtr<ICreateDevEnum> devs;
			HRESULT hRes = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&devs);
			if (FAILED(hRes)) return;
			TinyComPtr<IEnumMoniker> monikers;
			hRes = devs->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &monikers, 0);
			if (FAILED(hRes)) return;
			monikers->Reset();
			TinyComPtr<IMoniker> moniker;
			ULONG cFetched = 0;
			while (monikers->Next(1, &moniker, &cFetched) == S_OK)
			{
				string devname;
				TinyComPtr<IPropertyBag> bag;
				hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&bag);
				if (FAILED(hRes)) goto ERROR_LABEL;
				VARIANT val;
				hRes = bag->Read(L"Description", &val, 0);
				if (FAILED(hRes))
					hRes = bag->Read(L"FriendlyName", &val, 0);
				if (FAILED(hRes) || val.vt != VT_BSTR) goto ERROR_LABEL;
				IBaseFilter* ps = NULL;
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ps);
				if (FAILED(hRes)) goto ERROR_LABEL;
				devname = UTF16ToUTF8(V_BSTR(&val));
				if (!callback(devname.c_str(), &ps))
					break;
			ERROR_LABEL:
				moniker.Release();
			}
		}
	};
}
