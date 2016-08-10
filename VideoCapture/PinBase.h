#pragma once
#include "VideoCommon.h"

namespace Media
{
	class PinBase : public IPin, public IMemInputPin, public TinyReference < PinBase >
	{
		DISALLOW_COPY_AND_ASSIGN(PinBase);
	public:
		explicit PinBase(IBaseFilter* owner);
		~PinBase();
		void SetOwner(IBaseFilter* owner);
		virtual BOOL IsMediaTypeValid(const AM_MEDIA_TYPE* mediaType) = 0;
		virtual BOOL GetValidMediaType(INT index, AM_MEDIA_TYPE* mediaType) = 0;
	public:
		HRESULT STDMETHODCALLTYPE Connect(IPin *pReceivePin, _In_opt_ const AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Disconnect(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ConnectedTo(_Out_ IPin **pPin) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ConnectionMediaType(_Out_ AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryPinInfo(_Out_ PIN_INFO *pInfo) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryDirection(_Out_ PIN_DIRECTION *pPinDir) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryId(_Out_ LPWSTR *Id) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryAccept(const AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE EnumMediaTypes(_Out_ IEnumMediaTypes **ppEnum) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInternalConnections(_Out_writes_to_opt_(*nPin, *nPin) IPin **apPin, ULONG *nPin) OVERRIDE;
		HRESULT STDMETHODCALLTYPE EndOfStream(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE BeginFlush(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE EndFlush(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetAllocator(_Out_ IMemAllocator **ppAllocator) OVERRIDE;
		HRESULT STDMETHODCALLTYPE NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetAllocatorRequirements(_Out_ ALLOCATOR_PROPERTIES *pProps) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample) = 0;
		HRESULT STDMETHODCALLTYPE ReceiveMultiple(_In_reads_(nSamples) IMediaSample **pSamples, long nSamples, _Out_ long *nSamplesProcessed) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ReceiveCanBlock(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		AM_MEDIA_TYPE		m_mediaType;
		TinyComPtr<IPin>	m_connector;
		IBaseFilter*		m_ownerFilter;
		WCHAR*				m_pzName;
	};
}
