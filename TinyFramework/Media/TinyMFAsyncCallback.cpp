#include "../stdafx.h"
#include "TinyMFAsyncCallback.h"
#include <limits>
#include <algorithm>

namespace TinyFramework
{
	namespace Media
	{
		TinyMFAsyncCallback::TinyMFAsyncCallback()
			:m_pMFT(NULL),
			m_inputs(0),
			m_outputs(0)
		{

		}
		TinyMFAsyncCallback::~TinyMFAsyncCallback()
		{

		}
		BOOL TinyMFAsyncCallback::Initialize(IMFTransform* pMFT)
		{
			m_pMFT = pMFT;
			ASSERT(m_pMFT);
			HRESULT hRes = m_pMFT->QueryInterface(IID_PPV_ARGS(&m_eventGenerator));
			if (hRes != S_OK)
				return FALSE;
			hRes = m_eventGenerator->BeginGetEvent(this, NULL);
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
			HRESULT hStatus = S_OK;
			MediaEventType eventType = MEUnknown;
			IMFMediaEvent* pMFEvent = NULL;
			if (m_eventGenerator != NULL)
			{
				hRes = m_eventGenerator->EndGetEvent(pAsyncResult, &pMFEvent);
				if (hRes != S_OK)
					goto _ERROR;
				hRes = pMFEvent->GetType(&eventType);
				if (hRes != S_OK)
					goto _ERROR;
				hRes = pMFEvent->GetStatus(&hStatus);
				if (hRes != S_OK)
					goto _ERROR;
				if (SUCCEEDED(hStatus))
				{
					switch (eventType)
					{
					case METransformNeedInput:
						InterlockedIncrement(&m_inputs);
						OnInvokeInput();
						break;
					case METransformHaveOutput:
						InterlockedIncrement(&m_outputs);
						OnInvokeOutput();
						break;
					case METransformDrainComplete:
					{
						hRes = m_pMFT->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
						if (hRes != S_OK)
							goto _ERROR;
					}
					break;
					}
				}
				hRes = m_eventGenerator->BeginGetEvent(this, NULL);
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
