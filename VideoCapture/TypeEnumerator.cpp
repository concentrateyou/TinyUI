#include "stdafx.h"
#include "TypeEnumerator.h"

namespace Media
{
	TypeEnumerator::TypeEnumerator(PinBase* pin)
		:m_pin(pin)
	{
	}

	TypeEnumerator::~TypeEnumerator()
	{
	}
	void TypeEnumerator::FreeAllocatedMediaTypes(ULONG allocated, AM_MEDIA_TYPE** types)
	{
		for (ULONG i = 0; i < allocated; ++i)
		{
			CoTaskMemFree(types[i]->pbFormat);
			CoTaskMemFree(types[i]);
		}
	}
	HRESULT STDMETHODCALLTYPE TypeEnumerator::Next(ULONG cMediaTypes, _Out_writes_to_(cMediaTypes, *pcFetched) AM_MEDIA_TYPE **ppMediaTypes, _Out_opt_ ULONG *pcFetched)
	{
		ULONG fetched = 0;
		while (fetched < cMediaTypes)
		{
			AM_MEDIA_TYPE* type = reinterpret_cast<AM_MEDIA_TYPE*>(CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)));
			if (!type)
			{
				FreeAllocatedMediaTypes(fetched, ppMediaTypes);
				return E_OUTOFMEMORY;
			}
			ZeroMemory(type, sizeof(AM_MEDIA_TYPE));
			type->cbFormat = sizeof(VIDEOINFOHEADER);
			BYTE *format = reinterpret_cast<BYTE*>(CoTaskMemAlloc(sizeof(VIDEOINFOHEADER)));
			if (!format)
			{
				CoTaskMemFree(type);
				FreeAllocatedMediaTypes(fetched, ppMediaTypes);
				return E_OUTOFMEMORY;
			}
			type->pbFormat = format;
			if (m_pin->GetValidMediaType(m_index++, type))
			{
				ppMediaTypes[fetched++] = type;
			}
			else
			{
				CoTaskMemFree(format);
				CoTaskMemFree(type);
				break;
			}
		}
		if (pcFetched)
		{
			*pcFetched = fetched;
		}
		return fetched == cMediaTypes ? S_OK : S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Skip(ULONG cMediaTypes)
	{
		m_index += cMediaTypes;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Reset(void)
	{
		m_index = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Clone(_Out_ IEnumMediaTypes **ppEnum)
	{
		TypeEnumerator* ps = new TypeEnumerator(m_pin);
		ps->AddRef();
		ps->m_index = m_index;
		*ppEnum = ps;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IEnumMediaTypes) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IEnumMediaTypes*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE TypeEnumerator::AddRef(void)
	{
		TinyReference < TypeEnumerator >::AddRef();
		return 1;
		//return TinyReference < TypeEnumerator >::GetReference();
	}

	ULONG STDMETHODCALLTYPE TypeEnumerator::Release(void)
	{
		TinyReference < TypeEnumerator >::Release();
		return 1;
		//return TinyReference < TypeEnumerator >::GetReference();
	}

}
