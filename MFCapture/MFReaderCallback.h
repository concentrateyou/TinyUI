#pragma once
#include "MFCommon.h"

namespace MF
{
	class MFVideoCapture;

	class MFReaderCallback : public TinyReference<MFReaderCallback>, public IMFSourceReaderCallback
	{
	public:
		MFReaderCallback(MFVideoCapture* observer);
		virtual ~MFReaderCallback();
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
}

