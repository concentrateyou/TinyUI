#pragma once
#include <string>
#include "Network/TinySocket.h"
#include "Network/TinyDNS.h"
#include "Network/TinyURL.h"
#include "Network/TinyHTTPClient.h"
#include "IO/TinyIO.h"
#include "IO/TinyRingBuffer.h"
using namespace std;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Network;

namespace Decode
{
#define MAX_HTTP_BUFFER_SIZE		(2 * 1024 * 1024)
#define DEFAULT_CACHE_BLOCK_SIZE	(64 * 1024)
	/// <summary>
	/// FLV HTTP网络流(ps:微软的HTTP接口实在太慢)
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
		void SetErrorCallback(Callback<void(INT)>&& callback);
		BOOL Open(LPCSTR pzURL = "http://10.110.48.109:42948/6703234523&type=http&flashId=flv");
		BOOL Close();
	private:
		LONG					m_cRef;
		TinyHTTPClient			m_client;
	};
}

