#include "stdafx.h"
#include "FilterBase.h"

namespace Media
{
	FilterBase::FilterBase()
		:m_cRef(1)
	{

	}
	FilterBase::~FilterBase()
	{

	}
	HRESULT STDMETHODCALLTYPE FilterBase::EnumPins(_Out_ IEnumPins **ppEnum)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::FindPin(LPCWSTR Id, _Out_ IPin **ppPin)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryFilterInfo(_Out_ FILTER_INFO *pInfo)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::JoinFilterGraph(_In_opt_ IFilterGraph *pGraph, _In_opt_ LPCWSTR pName)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryVendorInfo(_Out_ LPWSTR *pVendorInfo)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Stop(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Pause(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Run(REFERENCE_TIME tStart)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetState(DWORD dwMilliSecsTimeout, _Out_ FILTER_STATE *State)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::SetSyncSource(_In_opt_ IReferenceClock *pClock)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetSyncSource(_Outptr_result_maybenull_ IReferenceClock **pClock)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetClassID(__RPC__out CLSID *pClassID)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	ULONG STDMETHODCALLTYPE FilterBase::AddRef(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	ULONG STDMETHODCALLTYPE FilterBase::Release(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}