#pragma once
#include "PinBase.h"

namespace DShow
{
	class TypeEnumerator : public IEnumMediaTypes, TinyFramework::TinyReference < TypeEnumerator >
	{
		DISALLOW_COPY_AND_ASSIGN(TypeEnumerator)
	public:
		explicit TypeEnumerator(PinBase* pPin);
		~TypeEnumerator();
		HRESULT STDMETHODCALLTYPE Next(ULONG cMediaTypes, _Out_writes_to_(cMediaTypes, *pcFetched) AM_MEDIA_TYPE **ppMediaTypes, _Out_opt_ ULONG *pcFetched) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Skip(ULONG cMediaTypes) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Reset(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Clone(_Out_ IEnumMediaTypes **ppEnum) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		void FreeAllocatedMediaTypes(ULONG allocated, AM_MEDIA_TYPE** types);
	private:
		INT			m_position;
		TinyFramework::TinyScopedReferencePtr<PinBase>	m_pin;
	};
}



