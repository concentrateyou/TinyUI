#include "stdafx.h"
#include "FilterBase.h"
#include "PinBase.h"
#include "PinEnumerator.h"

namespace Media
{
	FilterBase::FilterBase(LPWSTR pzName, REFCLSID  clsid, TinyLock* lock)
		:m_clsid(clsid),
		m_pzName(pzName),
		m_lock(lock),
		m_state(State_Stopped)
	{

	}
	FilterBase::~FilterBase()
	{

	}
	HRESULT FilterBase::StreamTime(ReferenceTime& rtStream)
	{
		if (m_clock == NULL)
		{
			return VFW_E_NO_CLOCK;
		}
		HRESULT hRes = m_clock->GetTime((REFERENCE_TIME*)&rtStream);
		if (FAILED(hRes))
		{
			return hRes;
		}
		rtStream -= m_start;
		return S_OK;
	}
	LPAMOVIESETUP_FILTER FilterBase::GetSetupData()
	{
		return  NULL;
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
		CheckPointer(ppPin, E_POINTER);
		TinyAutoLock lock(*m_lock);
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
		TinyAutoLock lock(*m_lock);
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
				if (pPin->IsConnect())
				{
					HRESULT hRes = pPin->OnActive(FALSE);
					if (hRes != NOERROR)
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
		TinyAutoLock lock(*m_lock);
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
				if (pPin->IsConnect())
				{
					HRESULT hRes = pPin->OnActive(TRUE);
					if (hRes != NOERROR)
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
		TinyAutoLock lock(*m_lock);
		m_start = tStart;
		if (m_state == State_Stopped)
		{
			HRESULT hRes = Pause();
			if (hRes != NOERROR)
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
				if (pPin->IsConnect())
				{
					HRESULT hRes = pPin->OnRun(tStart);
					if (hRes != NOERROR)
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
		UNREFERENCED_PARAMETER(dwMilliSecsTimeout);
		CheckPointer(State, E_POINTER);
		*State = m_state;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::SetSyncSource(_In_opt_ IReferenceClock *pClock)
	{
		TinyAutoLock lock(*m_lock);
		m_clock = pClock;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetSyncSource(_Outptr_result_maybenull_ IReferenceClock **pClock)
	{
		CheckPointer(pClock, E_POINTER);
		TinyAutoLock lock(*m_lock);
		*pClock = m_clock;
		(*pClock)->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FilterBase::GetClassID(__RPC__out CLSID *pClassID)
	{
		CheckPointer(pClassID, E_POINTER);
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
		else if (IsEqualIID(riid, IID_IAMovieSetup))
		{
			*ppvObject = static_cast<IAMovieSetup*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE FilterBase::Register()
	{
		LPAMOVIESETUP_FILTER psetupdata = GetSetupData();
		if (NULL == psetupdata)
			return S_FALSE;
		HRESULT hRes = CoInitialize((LPVOID)NULL);
		ASSERT(SUCCEEDED(hRes));
		IFilterMapper *pIFM = NULL;
		hRes = CoCreateInstance(CLSID_FilterMapper
			, NULL
			, CLSCTX_INPROC_SERVER
			, IID_IFilterMapper
			, (void **)&pIFM);
		if (SUCCEEDED(hRes))
		{
			hRes = AMovieSetupRegisterFilter(psetupdata, pIFM, TRUE);
			pIFM->Release();
		}
		CoFreeUnusedLibraries();
		CoUninitialize();
	}
	HRESULT STDMETHODCALLTYPE FilterBase::Unregister()
	{
		LPAMOVIESETUP_FILTER psetupdata = GetSetupData();
		if (NULL == psetupdata)
			return S_FALSE;
		HRESULT hRes = CoInitialize((LPVOID)NULL);
		ASSERT(SUCCEEDED(hRes));
		IFilterMapper *pIFM = NULL;
		hRes = CoCreateInstance(CLSID_FilterMapper
			, NULL
			, CLSCTX_INPROC_SERVER
			, IID_IFilterMapper
			, (void **)&pIFM);
		if (SUCCEEDED(hRes))
		{
			hRes = AMovieSetupRegisterFilter(psetupdata, pIFM, FALSE);
			pIFM->Release();
		}
		CoFreeUnusedLibraries();
		CoUninitialize();
		if (0x80070002 == hRes)
			return NOERROR;
		else
			return hRes;
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