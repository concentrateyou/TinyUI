#include "../stdafx.h"
#include "TinyMedia.h"

namespace TinyFramework
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
		AudioClientListener::AudioClientListener()
		{

		}
		AudioClientListener::~AudioClientListener()
		{
			if (m_enumerator != NULL)
			{
				m_enumerator->UnregisterEndpointNotificationCallback(this);
			}
			m_enumerator.Release();
		}
		BOOL AudioClientListener::Initialize(AudioClientCallback&& callback)
		{
			m_callback = std::move(callback);
			HRESULT hRes = m_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
			{
				if (hRes != CO_E_NOTINITIALIZED)
					return FALSE;
				hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
				if (SUCCEEDED(hRes))
					hRes = m_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			}
			hRes = m_enumerator->RegisterEndpointNotificationCallback(this);
			return hRes == S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState)
		{
			if (!m_callback.IsNull())
			{
				m_callback(pwstrDeviceId, 0);
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId)
		{
			if (!m_callback.IsNull())
			{
				m_callback(pwstrDeviceId, 1);
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId)
		{
			if (!m_callback.IsNull())
			{
				m_callback(pwstrDeviceId, 2);
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId)
		{
			if (!m_callback.IsNull())
			{
				m_callback(pwstrDefaultDeviceId, 3);
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key)
		{
			if (!m_callback.IsNull())
			{
				m_callback(pwstrDeviceId, 4);
			}
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE AudioClientListener::QueryInterface(REFIID riid, void **ppvObject)
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
		ULONG STDMETHODCALLTYPE AudioClientListener::AddRef(void)
		{
			TinyReference < AudioClientListener >::AddRef();
			return TinyReference < AudioClientListener >::GetReference();
		}
		ULONG STDMETHODCALLTYPE AudioClientListener::Release(void)
		{
			TinyReference < AudioClientListener >::Release();
			return TinyReference < AudioClientListener >::GetReference();
		}
		//////////////////////////////////////////////////////////////////////////
		MediaBuffer::MediaBuffer(DWORD dwMaxSize)
			:m_dwMaxSize(dwMaxSize),
			m_dwSize(0)
		{
			m_data.Reset(new BYTE[m_dwMaxSize]);
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
		BOOL WINAPI IsAsyncMFT(IMFTransform *pMFT, BOOL& bIsAsync)
		{
			if (!pMFT)
				return FALSE;
			TinyComPtr<IMFAttributes> attributes;
			bIsAsync = FALSE;
			UINT32 isAsync = 0;
			HRESULT hRes = S_OK;
			hRes = pMFT->GetAttributes(&attributes);
			if (SUCCEEDED(hRes))
			{
				hRes = attributes->GetUINT32(MF_TRANSFORM_ASYNC, &isAsync);
				bIsAsync = !!isAsync;
				return TRUE;
			}
			return FALSE;
		}
		BOOL WINAPI UnlockAsyncMFT(IMFTransform *pMFT)
		{
			if (!pMFT)
				return FALSE;
			TinyComPtr<IMFAttributes> attributes;
			UINT32 isAsync = 0;
			HRESULT hRes = S_OK;
			hRes = pMFT->GetAttributes(&attributes);
			if (SUCCEEDED(hRes))
			{
				hRes = attributes->SetUINT32(MF_TRANSFORM_ASYNC_UNLOCK, TRUE);
			}
			return SUCCEEDED(hRes);
		}
		IMFSample* WINAPI DuplicateSample(IMFSample* sapmle)
		{
			DWORD size1 = 0;
			DWORD size2 = 0;
			BYTE* bits1 = NULL;
			BYTE* bits2 = NULL;
			LONGLONG hnsSampleTime = 0;
			LONGLONG hnsSampleDuration = 0;
			IMFSample* duplicate = NULL;
			TinyComPtr<IMFMediaBuffer> buffer1;
			HRESULT hRes = sapmle->GetBufferByIndex(0, &buffer1);
			if (hRes != S_OK)
				return NULL;
			hRes = buffer1->GetCurrentLength(&size1);
			if (hRes != S_OK)
				return NULL;
			TinyComPtr<IMFMediaBuffer> buffer2;
			hRes = MFCreateMemoryBuffer(size1, &buffer2);
			if (hRes != S_OK)
				return NULL;
			hRes = buffer2->Lock(&bits2, NULL, NULL);
			if (hRes == S_OK)
			{
				hRes = buffer1->Lock(&bits1, NULL, NULL);
				if (hRes != S_OK)
				{
					buffer2->Unlock();
				}
				else
				{
					memcpy(bits2, bits1, size1);
					buffer1->Unlock();
				}
			}
			hRes = buffer2->SetCurrentLength(size1);
			if (hRes != S_OK)
				return NULL;
			hRes = MFCreateSample(&duplicate);
			if (hRes != S_OK)
				return NULL;
			hRes = duplicate->AddBuffer(buffer2);
			if (hRes != S_OK)
				return NULL;
			hRes = sapmle->GetSampleTime(&hnsSampleTime);
			if (hRes == S_OK)
			{
				hRes = duplicate->SetSampleTime(hnsSampleTime);
				if (hRes != S_OK)
					return NULL;
			}
			hRes = sapmle->GetSampleDuration(&hnsSampleDuration);
			if (hRes == S_OK)
			{
				hRes = duplicate->SetSampleDuration(hnsSampleDuration);
				if (hRes != S_OK)
					return NULL;
			}
			return duplicate;
		}
		//////////////////////////////////////////////////////////////////////////
		BOOL WINAPI GetAudioOutputType(REFCLSID clsid, IMFMediaType* inputType, const WAVEFORMATEX* pMFT, IMFMediaType** ppMediaType)
		{
			TinyComPtr<IUnknown> unknow;
			HRESULT hRes = unknow.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFTransform> transform;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&transform));
			if (hRes != S_OK)
				return FALSE;
			do
			{
				hRes = transform->SetInputType(0, inputType, 0);
				if (hRes != S_OK)
					return FALSE;
				if (clsid == CLSID_AACMFTEncoder)
				{
					DWORD dwTypeIndex = 0;
					TinyComPtr<IMFMediaType> mediaType;
					while (SUCCEEDED(transform->GetOutputAvailableType(0, dwTypeIndex++, &mediaType)))
					{

						UINT32 val = 0;
						mediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &val);
						if (val != pMFT->wBitsPerSample && val != 16)
							continue;
						mediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &val);
						if (val != pMFT->nSamplesPerSec)
							continue;
						mediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &val);
						if (val != pMFT->nChannels)
							continue;
						mediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &val);
						if (val != (pMFT->nAvgBytesPerSec / 8))
							continue;
						*ppMediaType = mediaType.Detach();
						return TRUE;
					}
					break;
				}
				if (clsid == CLSID_CMSAACDecMFT)
				{
					DWORD dwTypeIndex = 0;
					TinyComPtr<IMFMediaType> mediaType;
					while (SUCCEEDED(transform->GetOutputAvailableType(0, dwTypeIndex++, &mediaType)))
					{
						*ppMediaType = mediaType.Detach();
						return TRUE;
					}
					break;
				}
			} while (0);
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		MFSampleQueue::MFSampleQueue()
		{

		}
		MFSampleQueue::~MFSampleQueue()
		{

		}
		HRESULT STDMETHODCALLTYPE MFSampleQueue::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, __uuidof(IUnknown)) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = static_cast<IUnknown*>(this);
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}
		ULONG STDMETHODCALLTYPE MFSampleQueue::AddRef(void)
		{
			TinyReference < MFSampleQueue >::AddRef();
			return TinyReference < MFSampleQueue >::GetReference();
		}
		ULONG STDMETHODCALLTYPE MFSampleQueue::Release(void)
		{
			TinyReference < MFSampleQueue >::Release();
			return TinyReference < MFSampleQueue >::GetReference();
		}
		BOOL MFSampleQueue::Push(IMFSample*  sample)
		{
			TinyAutoLock lock(m_lock);
			if (!sample)
				return FALSE;
			sample->AddRef();
			return m_list.InsertLast(sample) != NULL;
		}
		BOOL MFSampleQueue::Pop(IMFSample** ps)
		{
			TinyAutoLock lock(m_lock);
			ITERATOR pos = m_list.First();
			if (pos != NULL)
			{
				IMFSample* sample = m_list.GetAt(pos);
				*ps = sample;
				return TRUE;
			}
			return FALSE;
		}
		void MFSampleQueue::Clear(void)
		{
			TinyAutoLock lock(m_lock);
			ITERATOR pos = m_list.First();
			while (pos != NULL)
			{
				IMFSample* sample = m_list.GetAt(pos);
				sample->Release();
				pos = m_list.Next(pos);
			}
			m_list.RemoveAll();
		}
		DWORD MFSampleQueue::GetSize()
		{
			return m_list.GetSize();
		}
		BOOL    MFSampleQueue::IsEmpty(void)
		{
			TinyAutoLock lock(m_lock);
			return m_list.IsEmpty();
		}
	};
}
