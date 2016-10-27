#pragma once
#include "FilterBase.h"

namespace DShow
{
	class PinEnumerator : public IEnumPins, public TinyReference < PinEnumerator >
	{
		DISALLOW_COPY_AND_ASSIGN(PinEnumerator)
	public:
		explicit PinEnumerator(FilterBase* pFilter);
		~PinEnumerator();
		HRESULT STDMETHODCALLTYPE Next(ULONG cPins, _Out_writes_to_(cPins, *pcFetched) IPin **ppPins, _Out_opt_ ULONG *pcFetched) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Skip(ULONG cPins) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Reset(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Clone(_Out_ IEnumPins **ppEnum) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		INT			m_position;
		INT			m_count;
		FilterBase* m_pFilter;
	};

}
