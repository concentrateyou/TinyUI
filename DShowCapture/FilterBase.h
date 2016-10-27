#pragma once
#include "DShowCommon.h"
#include "ReferenceTime.h"

namespace DShow
{
	class FilterBase : public IBaseFilter, public IAMovieSetup, public TinyUI::TinyReference < FilterBase >
	{
		friend class PinBase;
		DISALLOW_COPY_AND_ASSIGN(FilterBase)
	public:
		FilterBase(REFCLSID  clsid, TinyLock* lock);
		virtual ~FilterBase();
		virtual INT	GetPinCount() = 0;
		virtual IPin* GetPin(INT index) = 0;
		virtual HRESULT StreamTime(ReferenceTime& rtStream);
		virtual LPAMOVIESETUP_FILTER GetSetupData();
	public:
		IFilterGraph *GetFilterGraph();
		HRESULT ReconnectPin(IPin *pPin, const AM_MEDIA_TYPE* pmt);
	public:
		HRESULT STDMETHODCALLTYPE EnumPins(_Out_ IEnumPins **ppEnum) OVERRIDE;
		HRESULT STDMETHODCALLTYPE FindPin(LPCWSTR Id, _Out_ IPin **ppPin) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryFilterInfo(_Out_ FILTER_INFO *pInfo) OVERRIDE;
		HRESULT STDMETHODCALLTYPE JoinFilterGraph(_In_opt_ IFilterGraph *pGraph, _In_opt_ LPCWSTR pName) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryVendorInfo(_Out_ LPWSTR *pVendorInfo) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Stop(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Pause(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Run(REFERENCE_TIME tStart) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetState(DWORD dwMilliSecsTimeout, _Out_ FILTER_STATE *State) OVERRIDE;
		HRESULT STDMETHODCALLTYPE SetSyncSource(_In_opt_ IReferenceClock *pClock) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetSyncSource(_Outptr_result_maybenull_ IReferenceClock **pClock) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetClassID(__RPC__out CLSID *pClassID) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Register() OVERRIDE;
		HRESULT STDMETHODCALLTYPE Unregister() OVERRIDE;
	protected:
		FILTER_STATE				m_state;
		LPWSTR						m_pName;
		CLSID						m_clsid;
		ReferenceTime				m_start;
		TinyComPtr<IReferenceClock> m_clock;
		IFilterGraph*				m_pGraph;
		IMediaEventSink*			m_pSink;
		TinyLock*					m_lock;
	};
}
