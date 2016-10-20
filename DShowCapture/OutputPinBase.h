#pragma once
#pragma once
#include "PinBase.h"
#include "FilterBase.h"
#include "InputPinBase.h"

namespace Media
{
	class OutputPinBase : public PinBase
	{
	public:
		explicit OutputPinBase(FilterBase* pFilter, WCHAR* pzName, TinyLock* lock);
		virtual ~OutputPinBase();
		HRESULT CheckConnect(IPin *pPin) OVERRIDE;
		HRESULT OnConnect(IPin *pReceivePin) OVERRIDE;
		HRESULT OnDisconnect() OVERRIDE;
		HRESULT OnActive(BOOL active) OVERRIDE;
		HRESULT STDMETHODCALLTYPE EndOfStream(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE BeginFlush(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE EndFlush(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) OVERRIDE;
	public:
		virtual HRESULT GetAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);
		virtual HRESULT Allocate(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * ppropInputRequest) = 0;
		virtual HRESULT GetSample(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags);
		virtual HRESULT FillSample(IMediaSample *pSample);
		virtual HRESULT InitializeAllocator(IMemAllocator **ppAlloc);
	private:
		TinyComPtr<IMemAllocator>	m_allocator;
		TinyComPtr<IMemInputPin>	m_inputPin;
	};
}
