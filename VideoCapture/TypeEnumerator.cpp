#include "stdafx.h"
#include "TypeEnumerator.h"

namespace Media
{
	TypeEnumerator::TypeEnumerator()
	{
	}

	TypeEnumerator::~TypeEnumerator()
	{
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Next(ULONG cMediaTypes, _Out_writes_to_(cMediaTypes, *pcFetched) AM_MEDIA_TYPE **ppMediaTypes, _Out_opt_ ULONG *pcFetched)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Skip(ULONG cMediaTypes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Reset(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::Clone(_Out_ IEnumMediaTypes **ppEnum)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE TypeEnumerator::QueryInterface(REFIID riid, void **ppvObject)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	ULONG STDMETHODCALLTYPE TypeEnumerator::AddRef(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	ULONG STDMETHODCALLTYPE TypeEnumerator::Release(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}
