#include "stdafx.h"
#include "LAVCommon.h"

namespace LAV
{
	BOOL WINAPI GetFilterByCLSID(const string& clsid, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"CLSID", &variant, 0);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			string value = std::move(WStringToString(V_BSTR(&variant)));
			if (value.compare(clsid) == 0)
			{
				IBaseFilter* baseFilter = NULL;
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&baseFilter);
				if (SUCCEEDED(hRes) && baseFilter)
				{
					*ps = baseFilter;
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	ScopedBSTR::ScopedBSTR(const CHAR* psText)
		:m_bstr(NULL)
	{

	}
	ScopedBSTR::~ScopedBSTR()
	{

	}
}
