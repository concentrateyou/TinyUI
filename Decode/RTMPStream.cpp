#include "stdafx.h"
#include "RTMPStream.h"

namespace Decode
{
	RTMPStream::RTMPStream()
		:m_cRef(1),
		m_break(FALSE),
		m_ringBuffer(m_lock)
	{
		m_ringBuffer.Initialize(4 * 1024 * 1024, 1);
	}

	RTMPStream::~RTMPStream()
	{
		Close();
	}

	BOOL RTMPStream::Open(LPCSTR pzURL)
	{
		RTMP_Init(&m_sRTMP);
		if (!RTMP_SetupURL(&m_sRTMP, (CHAR*)pzURL))
			return FALSE;
		m_sRTMP.Link.timeout = 3000;//默认3秒超时
		m_sRTMP.Link.lFlags |= RTMP_LF_BUFX | RTMP_LF_FTCU | RTMP_LF_LIVE;
		RTMP_SetBufferMS(&m_sRTMP, 3600 * 1000);//1h
		if (!RTMP_Connect(&m_sRTMP, NULL))
			return FALSE;
		if (!RTMP_ConnectStream(&m_sRTMP, 0))
			return FALSE;
		m_bits.Reset(new CHAR[32 * 1024]);
		m_task.Submit(BindCallback(&RTMPStream::OnMessagePump, this));
		Sleep(1000);
		return TRUE;
	}

	BOOL RTMPStream::Close()
	{
		m_break = TRUE;
		m_task.Close(INFINITE);
		RTMP_Close(&m_sRTMP);
		return TRUE;
	}

	void RTMPStream::OnMessagePump()
	{
		do
		{
			if (!RTMP_IsConnected(&m_sRTMP))
				break;
			if (RTMP_IsTimedout(&m_sRTMP))
				break;
			INT s = RTMP_Read(&m_sRTMP, m_bits, 16 * 1024);
			if (s < 0)
				break;
			INT v = m_ringBuffer.Write(m_bits, s);
			if (v < s)
			{
				TRACE("RTMPStream - 写入RTMP数据已满\n");
			}
			else
			{
				TRACE("write数据:%d\n", s);
			}
		} while (!m_break);
	}

	STDMETHODIMP RTMPStream::QueryInterface(REFIID riid, void **ppvObj)
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

	STDMETHODIMP_(ULONG) RTMPStream::AddRef()
	{
		return InterlockedIncrement(&this->m_cRef);
	}

	STDMETHODIMP_(ULONG) RTMPStream::Release()
	{
		if (InterlockedDecrement(&this->m_cRef))
		{
			return this->m_cRef;
		}
		delete this;
		return NOERROR;
	}

	STDMETHODIMP RTMPStream::Read(VOID *pv, ULONG cb, ULONG *pcbRead)
	{
		ULONG   cbRead;
		HRESULT hRes = S_OK;
		if (!pv)
			return E_INVALIDARG;
		if (!pcbRead)
		{
			pcbRead = &cbRead;
		}
		*pcbRead = m_ringBuffer.Read(pv, cb);
		if (*pcbRead < cb)
		{
			TRACE("RTMPStream - 写入RTMP数据不够\n");
		}
		else
		{
			TRACE("read数据:%d\n", *pcbRead);
		}
		return S_OK;
	}

	STDMETHODIMP RTMPStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::SetSize(ULARGE_INTEGER libNewSize)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::CopyTo(IStream *pstmTo, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::Commit(DWORD grfCommitFlags)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::Revert()
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP RTMPStream::Clone(IStream * *ppstm)
	{
		return E_NOTIMPL;
	}
}