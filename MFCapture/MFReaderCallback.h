#pragma once
#include "MFCommon.h"

namespace MF
{
	class MFVideoCapture;

	class MFVideoReaderCallback : public TinyReference<MFVideoReaderCallback>, public IMFSourceReaderCallback
	{
	public:
		MFVideoReaderCallback(MFVideoCapture* observer);
		virtual ~MFVideoReaderCallback();
	public:
		HRESULT STDMETHODCALLTYPE OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample) OVERRIDE;
		HRESULT STDMETHODCALLTYPE OnFlush(_In_ DWORD dwStreamIndex) OVERRIDE;
		HRESULT STDMETHODCALLTYPE OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		MFVideoCapture* m_observer;
		TinyEvent		m_event;
	};

	class MFAudioCapture;

	class MFAudioReaderCallback : public TinyReference<MFAudioReaderCallback>, public IMFSourceReaderCallback
	{
	public:
		MFAudioReaderCallback(MFAudioCapture* observer);
		virtual ~MFAudioReaderCallback();
	public:
		HRESULT STDMETHODCALLTYPE OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags, _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample) OVERRIDE;
		HRESULT STDMETHODCALLTYPE OnFlush(_In_ DWORD dwStreamIndex) OVERRIDE;
		HRESULT STDMETHODCALLTYPE OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		MFAudioCapture* m_observer;
		TinyEvent		m_event;
	};
}

