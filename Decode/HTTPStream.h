#pragma once
#include <string>
#include "Network/TinySocket.h"
#include "Network/TinyDNS.h"
#include "Network/TinyURL.h"
#include "IO/TinyIO.h"
#include "IO/TinyRingBuffer.h"
#include "rtmp.h"
using namespace std;
using namespace TinyUI;
using namespace TinyUI::Network;

namespace Decode
{
	/// <summary>
	/// FLV HTTPÍøÂçÁ÷
	/// </summary>
	class HTTPStream : public IStream
	{
	public:
		// *** IUnknown methods ***
		STDMETHOD(QueryInterface) (REFIID riid, void **ppvObj);
		STDMETHOD_(ULONG, AddRef) ();
		STDMETHOD_(ULONG, Release) ();
		// *** IStream methods ***
		STDMETHOD(Read) (VOID *pv, ULONG cb, ULONG *pcbRead);
		STDMETHOD(Write) (VOID const *pv, ULONG cb, ULONG *pcbWritten);
		STDMETHOD(Seek) (LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
		STDMETHOD(SetSize) (ULARGE_INTEGER libNewSize);
		STDMETHOD(CopyTo) (IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
		STDMETHOD(Commit) (DWORD grfCommitFlags);
		STDMETHOD(Revert) ();
		STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		STDMETHOD(Stat) (STATSTG *pstatstg, DWORD grfStatFlag);
		STDMETHOD(Clone)(IStream **ppstm);
	public:
		HTTPStream();
		virtual~HTTPStream();
		BOOL Open(LPCSTR pzURL);
		BOOL Close();
	private:
		LONG			m_cRef;
		TinySocket		m_socket;
	};
}

