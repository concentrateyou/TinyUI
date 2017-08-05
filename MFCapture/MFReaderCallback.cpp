#include "stdafx.h"
#include "MFReaderCallback.h"
#include "MFVideoCapture.h"
#include "MFAudioCapture.h"

namespace MF
{
	MFVideoReaderCallback::MFVideoReaderCallback(MFVideoCapture* observer)
		:m_observer(observer)
	{
	}


	MFVideoReaderCallback::~MFVideoReaderCallback()
	{
	}

	HRESULT STDMETHODCALLTYPE MFVideoReaderCallback::OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample)
	{
		ASSERT(m_observer);
		HRESULT hRes = S_OK;
		if (FAILED(hrStatus))
		{
			hRes = hrStatus;
		}
		if (SUCCEEDED(hRes))
		{
			if (pSample != NULL)
			{
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = pSample->GetBufferByIndex(0, &buffer);
				if (hRes != S_OK)
					goto MFERROR;
				DWORD dwLength = 0;
				DWORD dwMaxLength = 0;
				BYTE* data = NULL;
				hRes = buffer->Lock(&data, &dwMaxLength, &dwLength);
				if (hRes != S_OK)
					goto MFERROR;
				m_observer->OnFrameReceive(data, dwLength, timeGetTime(), this);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					goto MFERROR;
				return hRes;
			}
		}
	MFERROR:
		m_observer->OnFrameReceive(NULL, 0, timeGetTime(), this);
		return hRes;
	}

	HRESULT STDMETHODCALLTYPE MFVideoReaderCallback::OnFlush(_In_ DWORD dwStreamIndex)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFVideoReaderCallback::OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFVideoReaderCallback::QueryInterface(REFIID riid, void **ppvObject)
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

	ULONG STDMETHODCALLTYPE MFVideoReaderCallback::AddRef(void)
	{
		TinyReference < MFVideoReaderCallback >::AddRef();
		return TinyReference < MFVideoReaderCallback >::GetReference();
	}

	ULONG STDMETHODCALLTYPE MFVideoReaderCallback::Release(void)
	{
		TinyReference < MFVideoReaderCallback >::Release();
		return TinyReference < MFVideoReaderCallback >::GetReference();
	}
	//////////////////////////////////////////////////////////////////////////
	MFAudioReaderCallback::MFAudioReaderCallback(MFAudioCapture* observer)
		:m_observer(observer)
	{
	}


	MFAudioReaderCallback::~MFAudioReaderCallback()
	{
	}

	HRESULT STDMETHODCALLTYPE MFAudioReaderCallback::OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample)
	{
		ASSERT(m_observer);
		HRESULT hRes = S_OK;
		if (FAILED(hrStatus))
		{
			hRes = hrStatus;
		}
		if (SUCCEEDED(hRes))
		{
			if (pSample != NULL)
			{
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = pSample->GetBufferByIndex(0, &buffer);
				if (hRes != S_OK)
					goto MFERROR;
				DWORD dwLength = 0;
				DWORD dwMaxLength = 0;
				BYTE* data = NULL;
				hRes = buffer->Lock(&data, &dwMaxLength, &dwLength);
				if (hRes != S_OK)
					goto MFERROR;
				m_observer->OnFrameReceive(data, dwLength, timeGetTime(), this);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					goto MFERROR;
				return hRes;
			}
		}
	MFERROR:
		m_observer->OnFrameReceive(NULL, 0, timeGetTime(), this);
		return hRes;
	}

	HRESULT STDMETHODCALLTYPE MFAudioReaderCallback::OnFlush(_In_ DWORD dwStreamIndex)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFAudioReaderCallback::OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MFAudioReaderCallback::QueryInterface(REFIID riid, void **ppvObject)
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

	ULONG STDMETHODCALLTYPE MFAudioReaderCallback::AddRef(void)
	{
		TinyReference < MFAudioReaderCallback >::AddRef();
		return TinyReference < MFAudioReaderCallback >::GetReference();
	}

	ULONG STDMETHODCALLTYPE MFAudioReaderCallback::Release(void)
	{
		TinyReference < MFAudioReaderCallback >::Release();
		return TinyReference < MFAudioReaderCallback >::GetReference();
	}
}
