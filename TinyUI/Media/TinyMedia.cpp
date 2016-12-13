#include "../stdafx.h"
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		TinyScopedAvrt::TinyScopedAvrt(LPCSTR pzTaskName)
			:m_hMM(NULL),
			m_dwTaskIndex(0)
		{
			m_hMM = AvSetMmThreadCharacteristics(pzTaskName, &m_dwTaskIndex);
		}
		TinyScopedAvrt::~TinyScopedAvrt()
		{
			if (m_hMM != NULL)
			{
				AvRevertMmThreadCharacteristics(m_hMM);
				m_hMM = NULL;
			}
		}
		TinyScopedAvrt::operator HANDLE() const
		{
			return m_hMM;
		}
		BOOL TinyScopedAvrt::SetPriority(AVRT_PRIORITY priority)
		{
			ASSERT(m_hMM);
			return AvSetMmThreadPriority(m_hMM, priority);
		}
		//////////////////////////////////////////////////////////////////////////
		AudioObserver::AudioObserver()
		{

		}
		AudioObserver::~AudioObserver()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioDeviceListener::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IMMNotificationClient)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IMMNotificationClient*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}
		ULONG STDMETHODCALLTYPE AudioDeviceListener::AddRef(void)
		{
			TinyReference < AudioDeviceListener >::AddRef();
			return TinyReference < AudioDeviceListener >::GetReference();
		}
		ULONG STDMETHODCALLTYPE AudioDeviceListener::Release(void)
		{
			TinyReference < AudioDeviceListener >::Release();
			return TinyReference < AudioDeviceListener >::GetReference();
		}
	};
}
