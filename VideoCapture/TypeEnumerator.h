#pragma once
#include "VideoCaptureDevice.h"

namespace Media
{
	class TypeEnumerator : public IEnumMediaTypes
	{
	public:
		TypeEnumerator();
		~TypeEnumerator();
		HRESULT STDMETHODCALLTYPE Next(ULONG cMediaTypes, _Out_writes_to_(cMediaTypes, *pcFetched) AM_MEDIA_TYPE **ppMediaTypes, _Out_opt_ ULONG *pcFetched) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Skip(ULONG cMediaTypes) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Reset(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Clone(_Out_ IEnumMediaTypes **ppEnum) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	};
}



