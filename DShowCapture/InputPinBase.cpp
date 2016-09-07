#include "stdafx.h"
#include "InputPinBase.h"
#include "PinEnumerator.h"

namespace Media
{
	InputPinBase::InputPinBase(FilterBase* pFilter, WCHAR* pzName, FilterObserver* observer)
		:PinBase(pFilter, PINDIR_INPUT, pzName),
		m_observer(observer),
		m_allocator(NULL),
		m_bReadOnly(FALSE)
	{

	}
	InputPinBase::~InputPinBase()
	{

	}
	HRESULT InputPinBase::CheckStreaming()
	{
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
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::GetAllocator(_Out_ IMemAllocator **ppAllocator)
	{
		if (m_allocator == NULL)
		{
			HRESULT hRes = CoCreateInstance(CLSID_MemoryAllocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IMemAllocator,
				(void **)&m_allocator);
			if (hRes != NOERROR)
			{
				return hRes;
			}
		}
		ASSERT(m_allocator != NULL);
		*ppAllocator = m_allocator;
		m_allocator->AddRef();
		return NOERROR;

	}
	HRESULT STDMETHODCALLTYPE InputPinBase::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
	{
		IMemAllocator *pOldAllocator = m_allocator;
		pAllocator->AddRef();
		m_allocator = pAllocator;
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
		if (hRes != NOERROR)
		{
			return hRes;
		}
		const INT size = pSample->GetActualDataLength();
		BYTE* data = NULL;
		if (FAILED(pSample->GetPointer(&data)))
			return S_FALSE;
		m_observer->OnFrameReceive(data, size, &m_mediaType);
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed)
	{
		HRESULT hRes = NOERROR;
		*nSamplesProcessed = 0;
		while (nSamples-- > 0)
		{
			hRes = Receive(pSamples[*nSamplesProcessed]);
			if (hRes != NOERROR)
			{
				break;
			}
			(*nSamplesProcessed)++;
		}
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::ReceiveCanBlock(void)
	{
		INT count = m_pFilter->GetPinCount();
		INT outputPins = 0;
		for (INT c = 0; c < count; c++)
		{
			IPin *pPin = m_pFilter->GetPin(c);
			if (NULL == pPin)
			{
				break;
			}
			PIN_DIRECTION pd;
			HRESULT hRes = pPin->QueryDirection(&pd);
			if (hRes != NOERROR)
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
					outputPins++;
					IMemInputPin *pInputPin = NULL;
					hRes = pConnector->QueryInterface(IID_IMemInputPin, (void **)&pInputPin);
					pConnector->Release();
					if (SUCCEEDED(hRes))
					{
						hRes = pInputPin->ReceiveCanBlock();
						pInputPin->Release();
						if (hRes != S_FALSE)
						{
							return NOERROR;
						}
					}
					else
					{
						return NOERROR;
					}
				}
			}
		}
		return outputPins == 0 ? NOERROR : S_FALSE;
	}
	HRESULT STDMETHODCALLTYPE InputPinBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IMemInputPin))
		{
			*ppvObject = static_cast<IMemInputPin*>(this);
			AddRef();
			return NOERROR;
		}
		return PinBase::QueryInterface(riid, ppvObject);
	}
	ULONG STDMETHODCALLTYPE InputPinBase::AddRef(void)
	{
		return PinBase::AddRef();
	}
	ULONG STDMETHODCALLTYPE InputPinBase::Release(void)
	{
		return PinBase::Release();
	}
}