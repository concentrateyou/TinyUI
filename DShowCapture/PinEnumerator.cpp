#include "stdafx.h"
#include "PinEnumerator.h"

namespace DShow
{
	PinEnumerator::PinEnumerator(FilterBase* pFilter)
		:m_pFilter(pFilter), m_position(0), m_count(0)
	{
		m_count = m_pFilter->GetPinCount();
	}


	PinEnumerator::~PinEnumerator()
	{

	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Next(ULONG cPins, _Out_writes_to_(cPins, *pcFetched) IPin **ppPins, _Out_opt_ ULONG *pcFetched)
	{
		ASSERT(m_pFilter);
		if (pcFetched != NULL)
		{
			*pcFetched = 0;
		}
		else if (cPins > 1)
		{
			return E_INVALIDARG;
		}
		ULONG cFetched = 0;
		while (cFetched < cPins && m_count > m_position)
		{
			IPin* pPin = m_pFilter->GetPin(m_position++);
			if (pPin == NULL)
			{
				ASSERT(cFetched == 0);
				return VFW_E_ENUM_OUT_OF_SYNC;
			}
			pPin->AddRef();
			ppPins[cFetched++] = pPin;
		}
		if (pcFetched)
		{
			*pcFetched = cFetched;
		}
		return cFetched == cPins ? S_OK : S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Skip(ULONG cPins)
	{
		ASSERT(m_pFilter);
		if ((m_count - m_position) > cPins)
		{
			m_position += cPins;
			return S_OK;
		}
		m_position = 0;
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Reset(void)
	{
		m_position = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinEnumerator::Clone(_Out_ IEnumPins **ppEnum)
	{
		ASSERT(m_pFilter);
		PinEnumerator* ps = new PinEnumerator(m_pFilter);
		ps->AddRef();
		ps->m_position = m_position;
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
