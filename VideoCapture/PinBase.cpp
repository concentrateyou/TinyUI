#include "stdafx.h"
#include "PinBase.h"
#include "TypeEnumerator.h"

namespace Media
{
	PinBase::PinBase(IBaseFilter* ownerFilter)
		:m_ownerFilter(ownerFilter)
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
		m_mediaType = *pmt;
		pReceivePin->AddRef();
		m_connector.Attach(pReceivePin);
		return pReceivePin->ReceiveConnection(this, pmt);
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
	{
		if (!IsMediaTypeValid(pmt))
			return VFW_E_TYPE_NOT_ACCEPTED;
		m_mediaType = *pmt;
		pConnector->AddRef();
		m_connector.Attach(pConnector);
		return S_OK;
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
		*pPinDir = PINDIR_INPUT;
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