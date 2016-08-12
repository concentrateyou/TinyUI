#pragma once
#include "VideoCommon.h"
#include "FilterObserver.h"

namespace Media
{
	class FilterBase;
	class PinBase : public IPin, public TinyReference < PinBase >
	{
		DISALLOW_COPY_AND_ASSIGN(PinBase);
	public:
		explicit PinBase(FilterBase* pFilter, PIN_DIRECTION dir, WCHAR* pzName);
		virtual ~PinBase();
		virtual HRESULT CheckMediaType(const AM_MEDIA_TYPE* mediaType) = 0;
		virtual HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* mediaType) = 0;
		virtual HRESULT SetMediaType(const AM_MEDIA_TYPE *mediaType);
		virtual HRESULT CheckConnect(IPin *pPin);
	public:
		HRESULT STDMETHODCALLTYPE Connect(IPin *pReceivePin, _In_opt_ const AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Disconnect(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ConnectedTo(_Out_ IPin **ppPin) OVERRIDE;
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
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	protected:
		HRESULT AgreeMediaType(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt);
		HRESULT TryConnection(IPin* pReceivePin, const AM_MEDIA_TYPE* pmt);
		HRESULT TryMediaTypes(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt, IEnumMediaTypes *pEnum);
		BOOL MediaTypeMatchPartial(const AM_MEDIA_TYPE *pmt1, const AM_MEDIA_TYPE *pmt2);
	protected:
		AM_MEDIA_TYPE		m_mediaType;
		PIN_DIRECTION		m_dir;
		TinyComPtr<IPin>	m_connector;
		FilterBase*			m_pFilter;
		WCHAR*				m_pzName;
		BOOL				m_bFlushing;
		REFERENCE_TIME		m_startTime;
		REFERENCE_TIME		m_stopTime;
		DOUBLE				m_rate;
	};
}
