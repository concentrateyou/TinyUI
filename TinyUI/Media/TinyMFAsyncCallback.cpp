#include "../stdafx.h"
#include "TinyMFAsyncCallback.h"
#include <limits>
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyMFAsyncCallback::TinyMFAsyncCallback()
			:m_pMFT(NULL),
			m_iCount(0),
			m_oCount(0)
		{

		}
		TinyMFAsyncCallback::~TinyMFAsyncCallback()
		{

		}
		BOOL TinyMFAsyncCallback::Initialize(IMFTransform* pMFT)
		{
			m_pMFT = pMFT;
			ASSERT(m_pMFT);
			HRESULT hRes = m_pMFT->QueryInterface(IID_PPV_ARGS(&m_event));
			if (hRes != S_OK)
				return FALSE;
			hRes = m_event->BeginGetEvent(this, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		HRESULT STDMETHODCALLTYPE TinyMFAsyncCallback::GetParameters(__RPC__out DWORD *pdwFlags, __RPC__out DWORD *pdwQueue)
		{
			return E_NOTIMPL;
		}


		HRESULT STDMETHODCALLTYPE TinyMFAsyncCallback::Invoke(__RPC__in_opt IMFAsyncResult *pAsyncResult)
		{
			HRESULT hRes = S_OK;
			HRESULT hrStatus = S_OK;
			MediaEventType eventType = MEUnknown;
			IMFMediaEvent* pMFEvent = NULL;
			if (m_event != NULL)
			{
				hRes = m_event->EndGetEvent(pAsyncResult, &pMFEvent);
				if (hRes != S_OK)
					goto _ERROR;
				hRes = pMFEvent->GetType(&eventType);
				if (hRes != S_OK)
					goto _ERROR;
				hRes = pMFEvent->GetStatus(&hrStatus);
				if (hRes != S_OK)
					goto _ERROR;
				if (SUCCEEDED(hrStatus))
				{
					switch (eventType)
					{
					case METransformNeedInput:
						InterlockedIncrement(&m_iCount);
						break;
					case METransformHaveOutput:
						InterlockedIncrement(&m_oCount);
						break;
					}
				}
				hRes = m_event->BeginGetEvent(this, NULL);
				if (hRes != S_OK)
					goto _ERROR;
			}
		_ERROR:
			SAFE_RELEASE(pMFEvent);
			return hRes;
		}

		HRESULT STDMETHODCALLTYPE TinyMFAsyncCallback::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IMFAsyncCallback)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IMFAsyncCallback*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}


		ULONG STDMETHODCALLTYPE TinyMFAsyncCallback::AddRef(void)
		{
			TinyReference < TinyMFAsyncCallback >::AddRef();
			return TinyReference < TinyMFAsyncCallback >::GetReference();
		}


		ULONG STDMETHODCALLTYPE TinyMFAsyncCallback::Release(void)
		{
			TinyReference < TinyMFAsyncCallback >::Release();
			return TinyReference < TinyMFAsyncCallback >::GetReference();
		}

	};
}
