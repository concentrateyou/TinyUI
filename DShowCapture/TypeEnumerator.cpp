#include "stdafx.h"
#include "TypeEnumerator.h"
#include "ScopedMediaType.h"

namespace DShow
{
	TypeEnumerator::TypeEnumerator(PinBase* pPin)
		:m_pin(pPin)
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
		if (pcFetched != NULL)
		{
			*pcFetched = 0;
		}
		else if (cMediaTypes > 1)
		{
			return E_INVALIDARG;
		}
		ULONG cFetched = 0;
		while (cFetched < cMediaTypes)
		{
			AM_MEDIA_TYPE* type = reinterpret_cast<AM_MEDIA_TYPE*>(CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)));
			if (!type)
			{
				FreeAllocatedMediaTypes(cFetched, ppMediaTypes);
				return E_OUTOFMEMORY;
			}
			ZeroMemory(type, sizeof(AM_MEDIA_TYPE));
			type->cbFormat = sizeof(VIDEOINFOHEADER);
			BYTE *format = reinterpret_cast<BYTE*>(CoTaskMemAlloc(sizeof(VIDEOINFOHEADER)));
			if (!format)
			{
				CoTaskMemFree(type);
				FreeAllocatedMediaTypes(cFetched, ppMediaTypes);
				return E_OUTOFMEMORY;
			}
			type->pbFormat = format;
			if (m_pin->GetMediaType(m_position++, type) == NOERROR)
			{
				ppMediaTypes[cFetched++] = type;
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
			*pcFetched = cFetched;
		}
		return cFetched == cMediaTypes ? S_OK : S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Skip(ULONG cMediaTypes)
	{
		if (cMediaTypes == 0)
		{
			return S_OK;
		}
		m_position += cMediaTypes;
		AM_MEDIA_TYPE cmt;
		ZeroMemory(&cmt, sizeof(cmt));
		cmt.cbFormat = sizeof(VIDEOINFOHEADER);
		HRESULT hRes = m_pin->GetMediaType(m_position - 1, &cmt);
		FreeMediaType(cmt);
		return hRes;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Reset(void)
	{
		m_position = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Clone(_Out_ IEnumMediaTypes **ppEnum)
	{
		TypeEnumerator* ps = new TypeEnumerator(m_pin);
		ps->AddRef();
		ps->m_position = m_position;
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
		return TinyReference < TypeEnumerator >::GetReference();
	}

	ULONG STDMETHODCALLTYPE TypeEnumerator::Release(void)
	{
		TinyReference < TypeEnumerator >::Release();
		return TinyReference < TypeEnumerator >::GetReference();
	}

}
