#pragma once
#include "PinBase.h"
#include "FilterBase.h"

namespace Media
{
	class InputPinBase : public PinBase, public IMemInputPin
	{
	public:
		InputPinBase(FilterBase* pFilter, WCHAR* pzName, FilterObserver* observer);
		virtual ~InputPinBase();
		virtual HRESULT CheckStreaming();
	public:
		HRESULT STDMETHODCALLTYPE GetAllocator(_Out_ IMemAllocator **ppAllocator) OVERRIDE;
		HRESULT STDMETHODCALLTYPE NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetAllocatorRequirements(_Out_ ALLOCATOR_PROPERTIES *pProps) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample);
		HRESULT STDMETHODCALLTYPE ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ReceiveCanBlock(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	protected:
		BOOL						m_bReadOnly;
		TinyComPtr<IMemAllocator>	m_allocator;
		AM_SAMPLE2_PROPERTIES		m_sampleProps;
		FilterObserver*				m_observer;
	};
}
