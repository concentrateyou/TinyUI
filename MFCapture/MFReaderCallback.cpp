#include "stdafx.h"
#include "MFReaderCallback.h"
#include "MFVideoCapture.h"

namespace MF
{
	MFReaderCallback::MFReaderCallback(MFVideoCapture* observer)
		:m_observer(observer)
	{
	}


	MFReaderCallback::~MFReaderCallback()
	{
	}

	HRESULT STDMETHODCALLTYPE MFReaderCallback::OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample)
	{
		ASSERT(m_observer);
		if (!pSample)
		{
			m_observer->OnFrameReceive(NULL, 0, 0, this);
		}
		DWORD count = 0;
		HRESULT hRes = pSample->GetBufferCount(&count);
		if (FAILED(hRes))
		{
			m_observer->OnError(hRes);
			return hRes;
		}
		for (DWORD i = 0; i < count; ++i)
		{
			TinyComPtr<IMFMediaBuffer> buffer;
			pSample->GetBufferByIndex(i, &buffer);
			if (buffer)
			{
				DWORD dwLength = 0;
				DWORD dwMaxLength = 0;
				BYTE* data = NULL;
				hRes = buffer->Lock(&data, &dwMaxLength, &dwLength);
				if (FAILED(hRes))
				{
					m_observer->OnError(hRes);
					return hRes;
				}
				m_observer->OnFrameReceive(data, dwLength, timeGetTime(), this);
				hRes = buffer->Unlock();
				if (FAILED(hRes))
				{
					m_observer->OnError(hRes);
					return hRes;
				}
			}
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFReaderCallback::OnFlush(_In_ DWORD dwStreamIndex)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFReaderCallback::OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFReaderCallback::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, __uuidof(IMFSourceReaderCallback)) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IMFSourceReaderCallback*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE MFReaderCallback::AddRef(void)
	{
		TinyReference < MFReaderCallback >::AddRef();
		return TinyReference < MFReaderCallback >::GetReference();
	}

	ULONG STDMETHODCALLTYPE MFReaderCallback::Release(void)
	{
		TinyReference < MFReaderCallback >::Release();
		return TinyReference < MFReaderCallback >::GetReference();
	}

}
