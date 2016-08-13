#include "stdafx.h"
#include "FilterBase.h"
#include "PinBase.h"
#include "PinEnumerator.h"

namespace Media
{
	FilterBase::FilterBase(REFCLSID  clsid, LPWSTR pzName)
		:m_clsid(clsid),
		m_pzName(pzName),
		m_state(State_Stopped)
	{

	}
	FilterBase::~FilterBase()
	{

	}
	HRESULT STDMETHODCALLTYPE FilterBase::EnumPins(_Out_ IEnumPins **ppEnum)
	{
		*ppEnum = new PinEnumerator(this);
		if (*ppEnum == NULL)
			return E_OUTOFMEMORY;
		(*ppEnum)->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::FindPin(LPCWSTR Id, _Out_ IPin **ppPin)
	{
		INT count = GetPinCount();
		for (INT i = 0; i < count; i++)
		{
			IPin *pPin = GetPin(i);
			if (NULL == pPin)
			{
				break;
			}
			LPWSTR id = NULL;
			if (SUCCEEDED(pPin->QueryId(&id)))
			{
				if (0 == StrCmpW(id, Id))
				{
					*ppPin = pPin;
					pPin->AddRef();
					CoTaskMemFree(id);
					return S_OK;
				}
				CoTaskMemFree(id);
			}
		}
		*ppPin = NULL;
		return VFW_E_NOT_FOUND;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryFilterInfo(_Out_ FILTER_INFO *pInfo)
	{
		memcpy(pInfo->achName, FILTER_NAME, sizeof(FILTER_NAME));
		pInfo->pGraph = m_graph;
		if (pInfo->pGraph)
		{
			pInfo->pGraph->AddRef();
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::JoinFilterGraph(_In_opt_ IFilterGraph *pGraph, _In_opt_ LPCWSTR pName)
	{
		m_graph = pGraph;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryVendorInfo(_Out_ LPWSTR *pVendorInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Stop(void)
	{
		if (m_state != State_Stopped)
		{
			INT count = GetPinCount();
			for (INT i = 0; i < count; i++)
			{
				PinBase *pPin = static_cast<PinBase*>(GetPin(i));
				if (NULL == pPin)
				{
					break;
				}
				if (pPin->IsConnected())
				{
					HRESULT hRes = pPin->OnActive(FALSE);
					if (FAILED(hRes))
					{
						return hRes;
					}
				}
			}
		}
		m_state = State_Stopped;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Pause(void)
	{
		if (m_state == State_Stopped)
		{
			INT count = GetPinCount();
			for (INT i = 0; i < count; i++)
			{
				PinBase *pPin = static_cast<PinBase*>(GetPin(i));
				if (!pPin)
				{
					break;
				}
				if (pPin->IsConnected())
				{
					HRESULT hRes = pPin->OnActive(TRUE);
					if (FAILED(hRes))
					{
						return hRes;
					}
				}
			}
		}
		m_state = State_Paused;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::Run(REFERENCE_TIME tStart)
	{
		m_start = tStart;
		if (m_state == State_Stopped)
		{
			HRESULT hRes = Pause();
			if (FAILED(hRes))
			{
				return hRes;
			}
		}
		if (m_state != State_Running)
		{
			INT count = GetPinCount();
			for (INT i = 0; i < count; i++)
			{
				PinBase *pPin = static_cast<PinBase*>(GetPin(i));
				if (NULL == pPin)
				{
					break;
				}
				if (pPin->IsConnected())
				{
					HRESULT hRes = pPin->OnRun(tStart);
					if (FAILED(hRes))
					{
						return hRes;
					}
				}
			}
		}
		m_state = State_Running;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetState(DWORD dwMilliSecsTimeout, _Out_ FILTER_STATE *State)
	{
		*State = m_state;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::SetSyncSource(_In_opt_ IReferenceClock *pClock)
	{
		m_clock = pClock;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetSyncSource(_Outptr_result_maybenull_ IReferenceClock **pClock)
	{
		*pClock = m_clock;
		(*pClock)->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetClassID(__RPC__out CLSID *pClassID)
	{
		*pClassID = m_clsid;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IMediaFilter) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IMediaFilter*>(this);
		}
		else if (IsEqualIID(riid, IID_IPersist))
		{
			*ppvObject = static_cast<IPersist*>(this);
		}
		else if (IsEqualIID(riid, IID_IBaseFilter))
		{
			*ppvObject = static_cast<IBaseFilter*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE FilterBase::AddRef(void)
	{
		TinyReference < FilterBase >::AddRef();
		return TinyReference < FilterBase >::GetReference();
	}

	ULONG STDMETHODCALLTYPE FilterBase::Release(void)
	{
		TinyReference < FilterBase >::Release();
		return TinyReference < FilterBase >::GetReference();
	}
}