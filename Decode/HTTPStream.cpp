#include "stdafx.h"
#include "HTTPStream.h"

namespace Decode
{
	HTTPStream::HTTPStream()
		:m_cRef(1),
		m_stream(NULL),
		m_bBreak(FALSE)
	{
	}

	HTTPStream::~HTTPStream()
	{
		Close();
	}

	BOOL HTTPStream::Open(LPCSTR pzURL)
	{
		m_buffer.Initialize(MAX_HTTP_BUFFER_SIZE, sizeof(BYTE));
		m_session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);      // 5秒的连接超时  
		m_session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 3000);         // 3秒的发送超时  
		m_session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 3000);      // 3秒的接收超时  
		m_session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 3000);    // 3秒的发送超时  
		m_session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 3000); // 3秒的接收超时  
		m_session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 5);
		INTERNET_SCHEME scheme;
		TinyString server;
		TinyString object;
		INTERNET_PORT port;
		if (ParseURL(pzURL, scheme, server, object, port))
		{
			TinyHTTPConnection* connection = m_session.GetHttpConnection(server.CSTR(), port, NULL, NULL);
			if (!connection)
				goto _ERROR;
			m_stream = connection->OpenRequest(TinyHTTPConnection::HTTP_VERB_GET, object.CSTR(),
				NULL,
				1,
				NULL,
				NULL,
				INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD);
			if (!m_stream)
				goto _ERROR;
			if (!m_stream->SendRequest())
				goto _ERROR;
			DWORD dwCode = 0;
			if (!m_stream->QueryInfoStatusCode(dwCode) || dwCode != HTTP_STATUS_OK)
				goto _ERROR;
			m_bits.Reset(new BYTE[DEFAULT_CACHE_BLOCK_SIZE * 4]);
			if (!m_bits)
				goto _ERROR;
			m_bBreak = FALSE;
			DownloadHTTP(DEFAULT_CACHE_BLOCK_SIZE * 4);
			WriteHTTP(DEFAULT_CACHE_BLOCK_SIZE * 4);
			return m_task.Submit(BindCallback(&HTTPStream::OnMessagePump, this));
		}
	_ERROR:
		this->Close();
		return FALSE;
	}

	BOOL HTTPStream::Close()
	{
		m_bBreak = TRUE;
		if (m_task.Close(INFINITE))
		{
			if (m_stream != NULL)
			{
				m_stream->Close();
			}
			m_session.Close();
			return TRUE;
		}
		return FALSE;
	}

	void HTTPStream::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			DownloadHTTP(DEFAULT_CACHE_BLOCK_SIZE);
			for (;;)
			{
				if (WriteHTTP(DEFAULT_CACHE_BLOCK_SIZE))
					break;
			}
		}
	}

	BOOL HTTPStream::WriteHTTP(DWORD dwSize)
	{
		TinyAutoLock lock(m_lock);
		if (m_buffer.GetAvailableOUT() <= DEFAULT_CACHE_BLOCK_SIZE && m_buffer.GetAvailableIN() >= DEFAULT_CACHE_BLOCK_SIZE)
		{
			DWORD dwRes = m_buffer.Write(m_bits, dwSize);
			ASSERT(dwRes == dwSize);
			return TRUE;
		}
		return FALSE;
	}

	BOOL HTTPStream::ReadHTTP(VOID *pv, ULONG cb, ULONG *pcbRead)
	{
		TinyAutoLock lock(m_lock);
		if (m_buffer.GetAvailableOUT() <= cb)
		{
			return FALSE;
		}
		*pcbRead = m_buffer.Read(pv, cb);
		return TRUE;
	}

	void HTTPStream::DownloadHTTP(DWORD dwSize)
	{
		ULONG offset = 0;
		BYTE* data = m_bits;
		for (;;)
		{
			if (offset >= dwSize)
				break;
			data += offset;
			ULONG size = m_stream->Read(data, dwSize - offset);
			offset += size;
		}
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
		for (;;)
		{
			if (ReadHTTP(pv, cb, pcbRead))
				break;
			Sleep(10);
		}
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