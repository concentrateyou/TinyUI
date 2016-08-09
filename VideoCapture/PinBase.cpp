#include "stdafx.h"
#include "PinBase.h"

namespace Media
{
	PinBase::PinBase()
		:m_cRef(1)
	{
	}


	PinBase::~PinBase()
	{

	}

	HRESULT STDMETHODCALLTYPE PinBase::Connect(IPin *pReceivePin, _In_opt_ const AM_MEDIA_TYPE *pmt)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::Disconnect(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::ConnectedTo(_Out_ IPin **pPin)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::ConnectionMediaType(_Out_ AM_MEDIA_TYPE *pmt)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryPinInfo(_Out_ PIN_INFO *pInfo)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryDirection(_Out_ PIN_DIRECTION *pPinDir)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryId(_Out_ LPWSTR *Id)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryAccept(const AM_MEDIA_TYPE *pmt)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::EnumMediaTypes(_Out_ IEnumMediaTypes **ppEnum)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::QueryInternalConnections(_Out_writes_to_opt_(*nPin, *nPin) IPin **apPin, ULONG *nPin)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::EndOfStream(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::BeginFlush(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::EndFlush(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::GetAllocator(_Out_ IMemAllocator **ppAllocator)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::GetAllocatorRequirements(_Out_ ALLOCATOR_PROPERTIES *pProps)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::Receive(IMediaSample *pSample)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE PinBase::ReceiveCanBlock(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
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
		return InterlockedIncrement(&this->m_cRef);
	}

	ULONG STDMETHODCALLTYPE PinBase::Release(void)
	{
		if (InterlockedDecrement(&this->m_cRef))
		{
			return this->m_cRef;
		}
		delete this;
		return NOERROR;
	}
}