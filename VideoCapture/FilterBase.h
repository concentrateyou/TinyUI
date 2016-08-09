#pragma once
#include "VideoCaptureDevice.h"

namespace Media
{
	class FilterBase : public IBaseFilter
	{
	public:
		FilterBase();
		~FilterBase();
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
	private:
		LONG  m_cRef;
	};
}
