#include "stdafx.h"
#include "PinEnumerator.h"

namespace Media
{
	PinEnumerator::PinEnumerator(FilterBase* pFilter)
		:m_pFilter(pFilter), m_index(0)
	{

	}


	PinEnumerator::~PinEnumerator()
	{

	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Next(ULONG cPins, _Out_writes_to_(cPins, *pcFetched) IPin **ppPins, _Out_opt_ ULONG *pcFetched)
	{
		ASSERT(m_pFilter);
		ULONG fectch = 0;
		while (fectch < cPins && m_pFilter->GetPinCount() > m_index)
		{
			IPin* pin = m_pFilter->GetPin(m_index++);
			pin->AddRef();
			ppPins[fectch++] = pin;
		}
		if (pcFetched)
		{
			*pcFetched = fectch;
		}
		return fectch == cPins ? S_OK : S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Skip(ULONG cPins)
	{
		ASSERT(m_pFilter);
		if ((m_pFilter->GetPinCount() - m_index) > cPins)
		{
			m_index += cPins;
			return S_OK;
		}
		m_index = 0;
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Reset(void)
	{
		m_index = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Clone(_Out_ IEnumPins **ppEnum)
	{
		ASSERT(m_pFilter);
		PinEnumerator* ps = new PinEnumerator(m_pFilter);
		ps->AddRef();
		ps->m_index = m_index;
		*ppEnum = ps;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IEnumPins) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IEnumPins*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE PinEnumerator::AddRef(void)
	{
		TinyReference < PinEnumerator >::AddRef();
		return TinyReference < PinEnumerator >::GetReference();
	}

	ULONG STDMETHODCALLTYPE PinEnumerator::Release(void)
	{
		TinyReference < PinEnumerator >::Release();
		return TinyReference < PinEnumerator >::GetReference();
	}

}
