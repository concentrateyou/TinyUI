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
		//////////////////////////////////////////////////////////////////////////
		MediaBuffer::MediaBuffer(DWORD dwMaxSize)
			:m_dwMaxSize(dwMaxSize),
			m_dwSize(0)
		{
			m_data.Reset(new BYTE[dwMaxSize]);
			memset(m_data, 0, dwMaxSize);
		}
		MediaBuffer::~MediaBuffer()
		{

		}
		HRESULT STDMETHODCALLTYPE MediaBuffer::SetLength(DWORD cbLength)
		{
			if (cbLength > m_dwMaxSize)
			{
				return E_INVALIDARG;
			}
			m_dwSize = cbLength;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE MediaBuffer::GetMaxLength(_Out_ DWORD *pcbMaxLength)
		{
			if (!pcbMaxLength)
			{
				return E_POINTER;
			}
			*pcbMaxLength = m_dwMaxSize;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE MediaBuffer::GetBufferAndLength(_Outptr_opt_result_bytebuffer_(*pcbLength) BYTE **ppBuffer, _Out_opt_ DWORD *pcbLength)
		{
			if (!ppBuffer && !pcbLength)
			{
				return E_POINTER;
			}
			if (ppBuffer)
			{
				*ppBuffer = m_data;
			}
			if (pcbLength)
			{
				*pcbLength = m_dwSize;
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE MediaBuffer::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IMediaBuffer)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IMediaBuffer*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}
		ULONG STDMETHODCALLTYPE MediaBuffer::AddRef(void)
		{
			TinyReference < MediaBuffer >::AddRef();
			return TinyReference < MediaBuffer >::GetReference();
		}
		ULONG STDMETHODCALLTYPE MediaBuffer::Release(void)
		{
			TinyReference < MediaBuffer >::Release();
			return TinyReference < MediaBuffer >::GetReference();
		}
		//////////////////////////////////////////////////////////////////////////
		void AudioOperations::StereoToMono(const BYTE* src, DWORD dwSamples, BYTE* dst)
		{
			DWORD count = dwSamples / 2;

			for (DWORD i = 0; i < count; i++)
			{
				
			}
		}
	};
}
