#include "stdafx.h"
#include "PinBase.h"
#include "TypeEnumerator.h"

namespace Media
{
	PinBase::PinBase(IBaseFilter* ownerFilter, PIN_DIRECTION pinDir)
		:m_ownerFilter(ownerFilter),
		m_pinDir(pinDir)
	{
		ZeroMemory(&m_mediaType, sizeof(m_mediaType));
	}

	PinBase::~PinBase()
	{

	}
	void PinBase::SetOwner(IBaseFilter* ownerFilter)
	{
		m_ownerFilter = ownerFilter;
	}
	HRESULT PinBase::SetMediaType(const AM_MEDIA_TYPE *mediaType)
	{
		if (&m_mediaType != mediaType)
		{
			FreeMediaType(m_mediaType);
			HRESULT hRes = CopyMediaType(&m_mediaType, mediaType);
			if (FAILED(hRes))
			{
				return E_OUTOFMEMORY;
			}
		}
		return S_OK;
	}
	HRESULT PinBase::CheckConnect(IPin *pPin)
	{
		PIN_DIRECTION pd;
		pPin->QueryDirection(&pd);
		ASSERT((pd == PINDIR_OUTPUT) || (pd == PINDIR_INPUT));
		ASSERT((m_pinDir == PINDIR_OUTPUT) || (m_pinDir == PINDIR_INPUT));
		if (pd == m_pinDir)
		{
			return VFW_E_INVALID_DIRECTION;
		}
		return S_OK;
	}
	HRESULT PinBase::AgreeMediaType(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt)
	{
		ASSERT(pReceivePin);
		IEnumMediaTypes *pEnumMediaTypes = NULL;
		if ((pmt != NULL) &&
			(pmt->majortype != GUID_NULL) &&
			(pmt->formattype != GUID_NULL))
		{
			return TryConnection(pReceivePin, pmt);
		}
		HRESULT hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
		for (INT i = 0; i < 2; i++)
		{
			HRESULT hRes;
			if (i == 0)
			{
				hRes = pReceivePin->EnumMediaTypes(&pEnumMediaTypes);
			}
			else
			{
				hRes = EnumMediaTypes(&pEnumMediaTypes);
			}
			if (SUCCEEDED(hRes))
			{
				ASSERT(pEnumMediaTypes);
				hRes = TryMediaTypes(pReceivePin, pmt, pEnumMediaTypes);
				pEnumMediaTypes->Release();
				if (SUCCEEDED(hRes))
				{
					return NOERROR;
				}
				else
				{
					if ((hRes != E_FAIL) &&
						(hRes != E_INVALIDARG) &&
						(hRes != VFW_E_TYPE_NOT_ACCEPTED))
					{
						hrFailure = hRes;
					}
				}
			}
		}
		return hrFailure;
	}
	HRESULT PinBase::TryConnection(IPin* pReceivePin, const AM_MEDIA_TYPE* pmt)
	{
		HRESULT hRes = CheckConnect(pReceivePin);
		if (FAILED(hRes))
		{
			return S_FALSE;
		}
		hRes = CheckMediaType(pmt);
		if (hRes == S_OK)
		{
			m_connector = pReceivePin;
			m_connector->AddRef();
			hRes = SetMediaType(pmt);
			if (SUCCEEDED(hRes))
			{
				hRes = pReceivePin->ReceiveConnection((IPin *)this, pmt);
				if (SUCCEEDED(hRes))
				{
					return S_OK;
				}
			}
		}
		else
		{
			if (SUCCEEDED(hRes) ||
				(hRes == E_FAIL) ||
				(hRes == E_INVALIDARG)) {
				hRes = VFW_E_TYPE_NOT_ACCEPTED;
			}
		}
		if (m_connector)
		{
			m_connector.Release();
		}
		return hRes;
	}
	HRESULT PinBase::TryMediaTypes(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt, IEnumMediaTypes *pEnum)
	{
		HRESULT hRes = pEnum->Reset();
		if (FAILED(hRes))
		{
			return hRes;
		}
		AM_MEDIA_TYPE *pMediaType = NULL;
		ULONG ulMediaCount = 0;
		HRESULT hrFailure = S_OK;
		for (;;)
		{

			hRes = pEnum->Next(1, (AM_MEDIA_TYPE**)&pMediaType, &ulMediaCount);
			if (hRes != S_OK)
			{
				if (S_OK == hrFailure)
				{
					hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
				}
				return hrFailure;
			}

			ASSERT(ulMediaCount == 1);
			ASSERT(pMediaType);
			if (pMediaType && ((pmt == NULL) || MediaTypeMatchPartial(pMediaType, pmt)))
			{
				hRes = TryConnection(pReceivePin, pMediaType);
				if (FAILED(hRes) &&
					SUCCEEDED(hrFailure) &&
					(hRes != E_FAIL) &&
					(hRes != E_INVALIDARG) &&
					(hRes != VFW_E_TYPE_NOT_ACCEPTED))
				{
					hrFailure = hRes;
				}
			}
			else
			{
				hRes = VFW_E_NO_ACCEPTABLE_TYPES;
			}

			if (pMediaType)
			{
				DeleteMediaType(pMediaType);
				pMediaType = NULL;
			}

			if (S_OK == hRes)
			{
				return hRes;
			}
		}
	}
	BOOL PinBase::MediaTypeMatchPartial(const AM_MEDIA_TYPE *pmt1, const AM_MEDIA_TYPE *pmt2)
	{
		if ((pmt1->majortype != GUID_NULL) &&
			(pmt2->majortype != GUID_NULL) &&
			(pmt2->majortype != pmt1->majortype))
		{
			return FALSE;
		}
		if ((pmt1->subtype != GUID_NULL) &&
			(pmt2->subtype != GUID_NULL) &&
			(pmt1->subtype != pmt2->subtype))
		{
			return FALSE;
		}

		if (pmt1->formattype != GUID_NULL &&
			pmt2->formattype != GUID_NULL)
		{
			if (pmt1->formattype != pmt2->formattype)
			{
				return FALSE;
			}
			if (pmt1->cbFormat != pmt2->cbFormat)
			{
				return FALSE;
			}
			if ((pmt1->cbFormat != 0) && (memcmp(pmt1->pbFormat, pmt2->pbFormat, pmt1->cbFormat) != 0))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	HRESULT STDMETHODCALLTYPE PinBase::Connect(IPin *pReceivePin, _In_opt_ const AM_MEDIA_TYPE *pmt)
	{
		if (!pReceivePin || !pmt)
		{
			return E_POINTER;
		}
		if (m_connector)
		{
			return VFW_E_ALREADY_CONNECTED;
		}
		HRESULT hRes = AgreeMediaType(pReceivePin, pmt);
		if (FAILED(hRes))
			return hRes;
		return S_OK;
		/*pReceivePin->AddRef();
		m_connector.Attach(pReceivePin);
		return pReceivePin->ReceiveConnection(this, pmt);*/
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
	{
		if (m_connector)
		{
			return VFW_E_ALREADY_CONNECTED;
		}
		HRESULT hRes = CheckConnect(pConnector);
		if (FAILED(hRes))
		{
			return hRes;
		}

		hRes = CheckMediaType(pmt);
		if (hRes != S_OK)
		{
			if (SUCCEEDED(hRes) || (hRes == E_FAIL) || (hRes == E_INVALIDARG))
			{
				hRes = VFW_E_TYPE_NOT_ACCEPTED;
			}
			return hRes;
		}
		m_connector = pConnector;
		m_connector->AddRef();
		hRes = SetMediaType(pmt);
		if (SUCCEEDED(hRes))
		{
			return NOERROR;
		}
		m_connector->Release();
		m_connector = NULL;
		return hRes;
	}

	HRESULT STDMETHODCALLTYPE PinBase::Disconnect(void)
	{
		if (!m_connector)
			return S_FALSE;
		m_connector.Release();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::ConnectedTo(_Out_ IPin **pPin)
	{
		*pPin = m_connector;
		if (!m_connector)
			return VFW_E_NOT_CONNECTED;
		m_connector->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::ConnectionMediaType(_Out_ AM_MEDIA_TYPE *pmt)
	{
		if (!m_connector)
			return VFW_E_NOT_CONNECTED;
		*pmt = m_mediaType;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryPinInfo(_Out_ PIN_INFO *pInfo)
	{
		memcpy(pInfo->achName, PIN_NAME, sizeof(PIN_NAME));
		pInfo->dir = PINDIR_INPUT;
		pInfo->pFilter = m_ownerFilter;
		if (m_ownerFilter)
		{
			m_ownerFilter->AddRef();
		}
		pInfo->achName[0] = L'\0';
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryDirection(_Out_ PIN_DIRECTION *pPinDir)
	{
		*pPinDir = m_pinDir;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryId(_Out_ LPWSTR *Id)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryAccept(const AM_MEDIA_TYPE *pmt)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinBase::EnumMediaTypes(_Out_ IEnumMediaTypes **ppEnum)
	{
		*ppEnum = new TypeEnumerator(this);
		(*ppEnum)->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryInternalConnections(_Out_writes_to_opt_(*nPin, *nPin) IPin **apPin, ULONG *nPin)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE PinBase::EndOfStream(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::BeginFlush(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::EndFlush(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE PinBase::GetAllocator(_Out_ IMemAllocator **ppAllocator)
	{
		return VFW_E_NO_ALLOCATOR;
	}

	HRESULT STDMETHODCALLTYPE PinBase::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PinBase::GetAllocatorRequirements(_Out_ ALLOCATOR_PROPERTIES *pProps)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed)
	{
		HRESULT hRes = S_OK;
		*nSamplesProcessed = 0;
		while (nSamples--)
		{
			hRes = Receive(pSamples[*nSamplesProcessed]);
			if (hRes != S_OK)
				break;
			++(*nSamplesProcessed);
		}
		return hRes;
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveCanBlock(void)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IPin) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IPin*>(this);
		}
		else if (IsEqualIID(riid, IID_IMemInputPin))
		{
			*ppvObject = static_cast<IMemInputPin*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE PinBase::AddRef(void)
	{
		TinyReference < PinBase >::AddRef();
		return TinyReference < PinBase >::GetReference();
	}

	ULONG STDMETHODCALLTYPE PinBase::Release(void)
	{
		TinyReference < PinBase >::Release();
		return TinyReference < PinBase >::GetReference();
	}
}