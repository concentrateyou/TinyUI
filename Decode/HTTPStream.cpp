#include "stdafx.h"
#include "HTTPStream.h"

namespace Decode
{
	HTTPStream::HTTPStream()
		:m_cRef(1)
	{
	}

	HTTPStream::~HTTPStream()
	{
		Close();
	}

	BOOL HTTPStream::Open(LPCSTR pzURL)
	{
		return FALSE;
	}

	BOOL HTTPStream::Close()
	{

		return TRUE;
	}

	STDMETHODIMP HTTPStream::QueryInterface(REFIID riid, void **ppvObj)
	{
		if (IsEqualIID(riid, IID_IStream) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObj = this;
		}
		else
		{
			*ppvObj = NULL;
			return E_NOINTERFACE;
		}
		return NOERROR;
	}

	STDMETHODIMP_(ULONG) HTTPStream::AddRef()
	{
		return InterlockedIncrement(&this->m_cRef);
	}

	STDMETHODIMP_(ULONG) HTTPStream::Release()
	{
		if (InterlockedDecrement(&this->m_cRef))
		{
			return this->m_cRef;
		}
		delete this;
		return NOERROR;
	}

	STDMETHODIMP HTTPStream::Read(VOID *pv, ULONG cb, ULONG *pcbRead)
	{
		return S_OK;
	}

	STDMETHODIMP HTTPStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::SetSize(ULARGE_INTEGER libNewSize)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::CopyTo(IStream *pstmTo, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::Commit(DWORD grfCommitFlags)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::Revert()
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP HTTPStream::Clone(IStream * *ppstm)
	{
		return E_NOTIMPL;
	}
}