#include "stdafx.h"
#include "FilterBase.h"
#include "PinBase.h"
#include "TypeEnumerator.h"

namespace Media
{
	PinBase::PinBase(FilterBase* pFilter, PIN_DIRECTION dir, WCHAR* pzName)
		:m_pFilter(pFilter),
		m_dir(dir),
		m_pzName(pzName),
		m_bFlushing(FALSE),
		m_startTime(0),
		m_stopTime(0),
		m_rate(0.0F)
	{
		ZeroMemory((void*)&m_mediaType, sizeof(m_mediaType));
	}
	PinBase::~PinBase()
	{

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
		ASSERT((m_dir == PINDIR_OUTPUT) || (m_dir == PINDIR_INPUT));
		if (pd == m_dir)
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
	}
	HRESULT STDMETHODCALLTYPE PinBase::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
	{
		if (!pConnector || !pmt)
		{
			return E_POINTER;
		}
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
	HRESULT STDMETHODCALLTYPE PinBase::ConnectedTo(_Out_ IPin **ppPin)
	{
		IPin *pPin = m_connector;
		*ppPin = pPin;
		if (!m_connector)
			return VFW_E_NOT_CONNECTED;
		m_connector->AddRef();
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::ConnectionMediaType(_Out_ AM_MEDIA_TYPE *pmt)
	{
		if (!m_connector)
		{
			ZeroMemory(pmt, sizeof(AM_MEDIA_TYPE));
			pmt->lSampleSize = 1;
			pmt->bFixedSizeSamples = TRUE;
			return VFW_E_NOT_CONNECTED;
		}
		CopyMediaType(pmt, &m_mediaType);
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryPinInfo(_Out_ PIN_INFO *pInfo)
	{
		pInfo->pFilter = m_pFilter;
		if (m_pFilter)
		{
			m_pFilter->AddRef();
		}
		if (m_pzName)
		{
			StringCchCopyW(pInfo->achName, ARRAYSIZE(pInfo->achName), m_pzName);
		}
		else
		{
			pInfo->achName[0] = L'\0';
		}
		pInfo->dir = m_dir;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryDirection(_Out_ PIN_DIRECTION *pPinDir)
	{
		*pPinDir = m_dir;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryId(_Out_ LPWSTR *Id)
	{
		size_t size;
		HRESULT hRes = StringCbLengthW(m_pzName, 100000, &size);
		if (FAILED(hRes))
			return hRes;
		*Id = (LPWSTR)CoTaskMemAlloc(size + sizeof(WCHAR));
		if (*Id == NULL)
		{
			return E_OUTOFMEMORY;
		}
		CopyMemory(*Id, m_pzName, size + sizeof(WCHAR));
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryAccept(const AM_MEDIA_TYPE *pmt)
	{
		HRESULT hRes = CheckMediaType(pmt);
		if (FAILED(hRes))
		{
			return S_FALSE;
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::EnumMediaTypes(_Out_ IEnumMediaTypes **ppEnum)
	{
		*ppEnum = new TypeEnumerator(this);
		if (*ppEnum == NULL)
			return E_OUTOFMEMORY;
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
		m_bFlushing = TRUE;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::EndFlush(void)
	{
		m_bFlushing = FALSE;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
	{
		m_startTime = tStart;
		m_stopTime = tStop;
		m_rate = dRate;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IPin) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IPin*>(this);
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
	//////////////////////////////////////////////////////////////////////////
	InputPinBase::InputPinBase(FilterBase* pFilter, WCHAR* pzName, FilterObserver* observer)
		:PinBase(pFilter, PINDIR_INPUT, pzName),
		m_observer(observer),
		m_pAllocator(NULL),
		m_bReadOnly(FALSE)
	{
		ZeroMemory((void*)&m_sampleProps, sizeof(m_sampleProps));
	}
	InputPinBase::~InputPinBase()
	{

	}
	HRESULT InputPinBase::CheckStreaming()
	{
		ASSERT(m_connector);
		FILTER_STATE state;
		m_pFilter->GetState(0, &state);
		if (state == State_Stopped)
		{
			return VFW_E_WRONG_STATE;
		}
		if (m_bFlushing)
		{
			return S_FALSE;
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::GetAllocator(_Out_ IMemAllocator **ppAllocator)
	{
		if (m_pAllocator == NULL)
		{
			HRESULT hRes = CoCreateInstance(CLSID_MemoryAllocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IMemAllocator,
				(void **)ppAllocator);
			if (FAILED(hRes))
			{
				return hRes;
			}
		}
		ASSERT(m_pAllocator != NULL);
		*ppAllocator = m_pAllocator;
		m_pAllocator->AddRef();
		return S_OK;

	}
	HRESULT STDMETHODCALLTYPE InputPinBase::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
	{
		IMemAllocator *pOldAllocator = m_pAllocator;
		pAllocator->AddRef();
		m_pAllocator = pAllocator;

		if (pOldAllocator != NULL)
		{
			pOldAllocator->Release();
		}
		m_bReadOnly = bReadOnly;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::GetAllocatorRequirements(_Out_ ALLOCATOR_PROPERTIES *pProps)
	{
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::Receive(IMediaSample *pSample)
	{
		ASSERT(m_observer);
		HRESULT hRes = CheckStreaming();
		if (FAILED(hRes))
		{
			return hRes;
		}
		const INT size = pSample->GetActualDataLength();
		BYTE* data = NULL;
		if (FAILED(pSample->GetPointer(&data)))
			return S_FALSE;
		m_observer->OnFrameReceive(data, size, NULL);
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed)
	{
		HRESULT hRes = S_OK;
		*nSamplesProcessed = 0;
		while (nSamples-- > 0)
		{
			hRes = Receive(pSamples[*nSamplesProcessed]);
			if (hRes != S_OK)
			{
				break;
			}
			(*nSamplesProcessed)++;
		}
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::ReceiveCanBlock(void)
	{
		INT cPins = m_pFilter->GetPinCount();
		INT cOutputPins = 0;
		for (INT c = 0; c < cPins; c++)
		{
			IPin *pPin = m_pFilter->GetPin(c);
			if (NULL == pPin)
			{
				break;
			}
			PIN_DIRECTION pd;
			HRESULT hRes = pPin->QueryDirection(&pd);
			if (FAILED(hRes))
			{
				return hRes;
			}
			if (pd == PINDIR_OUTPUT)
			{
				IPin *pConnector = NULL;
				hRes = pPin->ConnectedTo(&pConnector);
				if (SUCCEEDED(hRes))
				{
					ASSERT(pConnector != NULL);
					cOutputPins++;
					IMemInputPin *pInputPin = NULL;
					hRes = pConnector->QueryInterface(IID_IMemInputPin, (void **)&pInputPin);
					pConnector->Release();
					if (SUCCEEDED(hRes))
					{
						hRes = pInputPin->ReceiveCanBlock();
						pInputPin->Release();
						if (hRes != S_FALSE)
						{
							return S_OK;
						}
					}
					else
					{
						return S_OK;
					}
				}
			}
		}
		return cOutputPins == 0 ? S_OK : S_FALSE;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IMemInputPin) && IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IMemInputPin*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}
	ULONG STDMETHODCALLTYPE InputPinBase::AddRef(void)
	{
		TinyReference < InputPinBase >::AddRef();
		return TinyReference < InputPinBase >::GetReference();
	}
	ULONG STDMETHODCALLTYPE InputPinBase::Release(void)
	{
		TinyReference < InputPinBase >::Release();
		return TinyReference < InputPinBase >::GetReference();
	}
}