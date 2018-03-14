#include "stdafx.h"
#include "HTTPStream.h"
#include "Common/TinyLogging.h"
#include "Common/TinyTime.h"

namespace Decode
{
	HTTPStream::HTTPStream()
		:m_cRef(1),
		m_size(DEFAULT_HTTP_BUFFER_SIZE)
	{
		m_raw.Reset(new CHAR[m_size]);
		ZeroMemory(m_raw, m_size);
	}

	HTTPStream::~HTTPStream()
	{
		Close();
	}

	BOOL HTTPStream::Open(LPCSTR pzURL)
	{
		return m_client.Open(pzURL);
	}

	BOOL HTTPStream::Close()
	{
		m_client.Close();
		return TRUE;
	}
	void HTTPStream::SetErrorCallback(ErrorCallback&& callback)
	{
		m_client.SetErrorCallback(std::move(callback));
	}

	HTTPResponse& HTTPStream::GetResponse()
	{
		return m_client.GetResponse();
	}

	HTTPRequest& HTTPStream::GetRequest()
	{
		return m_client.GetRequest();
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
		if (!pv)
			return E_INVALIDARG;
		ULONG cbRead = 0;
		if (!pcbRead)
		{
			pcbRead = &cbRead;
		}
		if (cb > DEFAULT_HTTP_BUFFER_SIZE)
		{
			m_size = cb;
			m_raw.Reset(new CHAR[m_size]);
		}
		HRESULT hRes = S_OK;
		INT size = 0;
		size = m_client.Read(m_raw, cb);
		if (size <= 0)
		{
			hRes = E_FAIL;
		}
		if (size > 0)
		{
			memcpy(pv, m_raw, size);
		}
		*pcbRead = static_cast<ULONG>(size);
		return hRes;
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